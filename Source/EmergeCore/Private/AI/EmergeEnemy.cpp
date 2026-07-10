#include "AI/EmergeEnemy.h"
#include "AI/EmergeEnemyController.h"
#include "AlsCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Settings/AlsMantlingSettings.h"
#include "Utility/AlsGameplayTags.h"
#include "UObject/ConstructorHelpers.h"

AEmergeEnemy::AEmergeEnemy()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEmergeEnemyController::StaticClass();

	// ALS body: same mesh + anim blueprint family as the player -> identical animation quality.
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
			TEXT("/ALS/ALS/Character/SKM_Als.SKM_Als"));
		if (MeshAsset.Succeeded()) { MeshComp->SetSkeletalMesh(MeshAsset.Object); }
		static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/ALS/ALS/Character/AB_Als"));
		if (AnimBP.Succeeded()) { MeshComp->SetAnimInstanceClass(AnimBP.Class); }
	}

	// ALS gameplay settings (required or the animation refresh early-returns).
	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> AlsSettings(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (AlsSettings.Succeeded()) { Settings = AlsSettings.Object; }

	// Base movement asset (curves source); duplicated + speed-overridden in PostInitializeComponents
	// — the supported runtime pattern (a CDO-subobject DataAsset copy is not the idiom; researched).
	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> BaseMovement(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (BaseMovement.Succeeded()) { BaseMovementSettings = BaseMovement.Object; }

	// Same mantle as the player (montages + auto start time).
	MantlingSettingsHigh = CreateDefaultSubobject<UAlsMantlingSettings>(TEXT("MantlingSettingsHigh"));
	MantlingSettingsLow = CreateDefaultSubobject<UAlsMantlingSettings>(TEXT("MantlingSettingsLow"));
	MantlingSettingsHigh->bAutoCalculateStartTime = true;
	MantlingSettingsLow->bAutoCalculateStartTime = true;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MantleHigh(
		TEXT("/ALS/ALS/Animations/Actions/Mantle/AM_Als_Mantle_High.AM_Als_Mantle_High"));
	if (MantleHigh.Succeeded()) { MantlingSettingsHigh->Montage = MantleHigh.Object; }
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MantleLow(
		TEXT("/ALS/ALS/Animations/Actions/Mantle/AM_Als_Mantle_Low.AM_Als_Mantle_Low"));
	if (MantleLow.Succeeded()) { MantlingSettingsLow->Montage = MantleLow.Object; }
}

void AEmergeEnemy::PostInitializeComponents()
{
	// Zombie tuning: duplicate MS_Als_Normal and override BOTH the gait speeds (Walking = shamble,
	// Running = chase, from the Zombie*Speed properties) and the accel/decel/friction curve — ALS
	// recomputes MaxWalkSpeed/MaxAcceleration/braking every physics tick from this gait table +
	// curves, so direct CMC field writes are dead tuning; everything goes through this object.
	// DuplicateObject copies the table but SHARES the referenced UCurveVector asset, so the curve
	// gets its own duplicate too: X channel = acceleration, scaled by 0.35 for weighty zombie
	// starts (Y = deceleration and Z = ground friction stay stock — stops/traction unchanged).
	// Must happen BEFORE Super, which hands MovementSettings to the movement component.
	if (BaseMovementSettings)
	{
		UAlsMovementSettings* Zombie = DuplicateObject(BaseMovementSettings.Get(), this, TEXT("ZombieMovementSettings"));
		TMap<UCurveVector*, UCurveVector*> DuplicatedCurves;   // stances usually share one curve asset: duplicate once
		for (TPair<FGameplayTag, FAlsMovementStanceSettings>& RotationPair : Zombie->RotationModes)
		{
			for (TPair<FGameplayTag, FAlsMovementGaitSettings>& StancePair : RotationPair.Value.Stances)
			{
				StancePair.Value.WalkForwardSpeed = ZombieWalkSpeed;
				StancePair.Value.WalkBackwardSpeed = ZombieWalkSpeed;
				StancePair.Value.RunForwardSpeed = ZombieRunSpeed;
				StancePair.Value.RunBackwardSpeed = ZombieRunSpeed;
				StancePair.Value.SprintSpeed = ZombieSprintSpeed;
				if (UCurveVector* SourceCurve = StancePair.Value.AccelerationAndDecelerationAndGroundFrictionCurve)
				{
					UCurveVector*& Scaled = DuplicatedCurves.FindOrAdd(SourceCurve);
					if (!Scaled)
					{
						Scaled = DuplicateObject(SourceCurve, this);
						for (FRichCurveKey& Key : Scaled->FloatCurves[0].Keys)   // [0] = X = acceleration
						{
							Key.Value *= 0.35f;
						}
					}
					StancePair.Value.AccelerationAndDecelerationAndGroundFrictionCurve = Scaled;
				}
			}
		}
		ZombieMovementSettings = Zombie;
		MovementSettings = Zombie;
	}
	Super::PostInitializeComponents();
}

void AEmergeEnemy::SetChaseSpeedScale(float Scale)
{
	Scale = FMath::Clamp(Scale, 0.5f, 1.0f);
	// Re-apply only on a real change: rewriting the gait table + RefreshGaitSettings every tick
	// for sub-percent wiggle is pointless churn.
	if (!ZombieMovementSettings || FMath::Abs(Scale - LastChaseSpeedScale) <= 0.02f) { return; }
	LastChaseSpeedScale = Scale;
	const float ScaledRunSpeed = ZombieRunSpeed * Scale;
	for (TPair<FGameplayTag, FAlsMovementStanceSettings>& RotationPair : ZombieMovementSettings->RotationModes)
	{
		for (TPair<FGameplayTag, FAlsMovementGaitSettings>& StancePair : RotationPair.Value.Stances)
		{
			StancePair.Value.RunForwardSpeed = ScaledRunSpeed;
			StancePair.Value.RunBackwardSpeed = ScaledRunSpeed;
		}
	}
	// ALS caches the gait settings in the movement component and recomputes MaxWalkSpeed from that
	// cache every physics tick — the edit above is invisible until SetMovementSettings re-hands the
	// object over and forces RefreshGaitSettings.
	if (UAlsCharacterMovementComponent* Move = Cast<UAlsCharacterMovementComponent>(GetCharacterMovement()))
	{
		Move->SetMovementSettings(ZombieMovementSettings);
	}
}

void AEmergeEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Face travel direction (an AI has no meaningful view direction) and start at shamble.
	// NOTE: the desired rotation mode applies on the next locomotion refresh — there is a single
	// frame of ViewDirection facing first (cosmetic one-frame snap; documented so nobody chases it).
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
	SetDesiredGait(AlsGaitTags::Walking);
	// De-robotize (research quick wins): injured overlay = lurchy full-body posture for free.
	// Weighty starts come from the 0.35x-scaled acceleration curve baked into the duplicated
	// movement settings in PostInitializeComponents — NOT from CMC field writes here: ALS
	// recomputes MaxAcceleration/braking from its gait curves every physics tick (and the ALS
	// ctor already enables bUseAccelerationForPaths), so the old direct writes were dead code.
	SetOverlayMode(AlsOverlayModeTags::Injured);
}

UAlsMantlingSettings* AEmergeEnemy::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return (MantlingType == EAlsMantlingType::Low) ? MantlingSettingsLow : MantlingSettingsHigh;
}

FString AEmergeEnemy::GetAnimDebug() const
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	const UAnimInstance* Anim = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	return FString::Printf(TEXT("{\"anim\":%s,\"gait\":\"%s\",\"action\":\"%s\",\"spd\":%.0f}"),
		Anim ? *FString::Printf(TEXT("\"%s\""), *Anim->GetClass()->GetName()) : TEXT("null"),
		*GetDesiredGait().GetTagName().ToString(),
		*GetLocomotionAction().GetTagName().ToString(), GetVelocity().Size2D());
}
