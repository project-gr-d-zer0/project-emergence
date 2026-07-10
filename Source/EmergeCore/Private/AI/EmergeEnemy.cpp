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

	// Same mantle as the player (montages + auto start time) — kept as the non-zombie fallback.
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

	// Zombie mantle VARIETY bank: 3 rate-scaled duplicates per height (0.50/0.60/0.72, built into
	// /Game/ZombieAnim by emerge_py/make_zombie_mantles.py — duplicates inherit the source montage
	// slots, so ALS mantling slot expectations are unchanged). A variant whose montage asset is
	// missing is NOT added, so SelectMantlingSettings can trust every array element and cleanly
	// falls back to the single settings when the bank is empty (script not run yet).
	static const TCHAR* ZombieMantlePaths[2][3] = {
		{
			TEXT("/Game/ZombieAnim/AM_Zombie_Mantle_High_1.AM_Zombie_Mantle_High_1"),
			TEXT("/Game/ZombieAnim/AM_Zombie_Mantle_High_2.AM_Zombie_Mantle_High_2"),
			TEXT("/Game/ZombieAnim/AM_Zombie_Mantle_High_3.AM_Zombie_Mantle_High_3")
		},
		{
			TEXT("/Game/ZombieAnim/AM_Zombie_Mantle_Low_1.AM_Zombie_Mantle_Low_1"),
			TEXT("/Game/ZombieAnim/AM_Zombie_Mantle_Low_2.AM_Zombie_Mantle_Low_2"),
			TEXT("/Game/ZombieAnim/AM_Zombie_Mantle_Low_3.AM_Zombie_Mantle_Low_3")
		}
	};
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieHigh1(ZombieMantlePaths[0][0]);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieHigh2(ZombieMantlePaths[0][1]);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieHigh3(ZombieMantlePaths[0][2]);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieLow1(ZombieMantlePaths[1][0]);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieLow2(ZombieMantlePaths[1][1]);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ZombieLow3(ZombieMantlePaths[1][2]);
	UAnimMontage* const ZombieHighMontages[3] = { ZombieHigh1.Object, ZombieHigh2.Object, ZombieHigh3.Object };
	UAnimMontage* const ZombieLowMontages[3] = { ZombieLow1.Object, ZombieLow2.Object, ZombieLow3.Object };
	for (int32 Index = 0; Index < 3; ++Index)
	{
		if (ZombieHighMontages[Index])
		{
			UAlsMantlingSettings* SettingsObject = CreateDefaultSubobject<UAlsMantlingSettings>(
				*FString::Printf(TEXT("ZombieMantleHigh%d"), Index + 1));
			SettingsObject->bAutoCalculateStartTime = true;
			SettingsObject->Montage = ZombieHighMontages[Index];
			ZombieMantleHigh.Add(SettingsObject);
		}
		if (ZombieLowMontages[Index])
		{
			UAlsMantlingSettings* SettingsObject = CreateDefaultSubobject<UAlsMantlingSettings>(
				*FString::Printf(TEXT("ZombieMantleLow%d"), Index + 1));
			SettingsObject->bAutoCalculateStartTime = true;
			SettingsObject->Montage = ZombieLowMontages[Index];
			ZombieMantleLow.Add(SettingsObject);
		}
	}

	// Zombie look default: MoCap Online Zombie Pro mesh (UE4-Mannequin rig). Soft path only —
	// loaded at runtime in SetupZombieLook via LoadSynchronous, NOT ConstructorHelpers (a hard
	// find here would drag the asset into memory with the CDO even with bUseZombieLook off).
	ZombieMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(
		TEXT("/Game/Zombie_01/Character/Mesh/SK_Mannequin.SK_Mannequin")));
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

	// Zombie look (v2 compatible-skeletons): swap the mesh BEFORE Super — AAlsCharacter::
	// PostInitializeComponents caches AnimationInstance from GetMesh()->GetAnimInstance(), and
	// SetSkeletalMeshAsset re-creates the anim instance; swapping here means ALS caches the
	// AB_Als instance that lives on the ZOMBIE mesh (swapping later would strand a stale cache
	// and dead-tick the whole ALS pipeline — the v1 failure mode).
	if (bUseZombieLook)
	{
		SetupZombieLook();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EMERGE_ZLOOK: %s: bUseZombieLook=false - staying on the ALS body"), *GetName());
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

void AEmergeEnemy::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// EMERGE_ZLOOK heartbeat (2s cadence): ground truth that the ALS pipeline is alive on the
	// swapped mesh — mesh asset, anim instance CLASS (must be AB_Als_C, not null), gait, speed.
	if (const UWorld* World = GetWorld(); World && World->GetTimeSeconds() >= ZombieNextDebugTime)
	{
		ZombieNextDebugTime = World->GetTimeSeconds() + 2.0;
		const USkeletalMeshComponent* MeshComp = GetMesh();
		const UAnimInstance* Anim = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
		UE_LOG(LogTemp, Warning,
			TEXT("EMERGE_ZLOOK: %s: heartbeat zlook=%d mesh=%s animInstance=%s gait=%s action=%s spd=%.0f rendered=%d"),
			*GetName(), bZombieLookActive ? 1 : 0,
			MeshComp && MeshComp->GetSkeletalMeshAsset() ? *MeshComp->GetSkeletalMeshAsset()->GetName() : TEXT("NULL"),
			Anim ? *Anim->GetClass()->GetName() : TEXT("NULL"),
			*GetDesiredGait().GetTagName().ToString(),
			*GetLocomotionAction().GetTagName().ToString(),
			GetVelocity().Size2D(),
			MeshComp && MeshComp->bRecentlyRendered ? 1 : 0);
	}
}

void AEmergeEnemy::SetupZombieLook()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	USkeletalMesh* Zombie = ZombieMesh.LoadSynchronous();
	if (!MeshComp || !Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("EMERGE_ZLOOK: %s: FAIL - %s - keeping the ALS body"),
			*GetName(), !MeshComp ? TEXT("GetMesh() is null") : TEXT("ZombieMesh.LoadSynchronous() returned null"));
		return;
	}

	// The whole v2 look: swap the mesh asset, change NOTHING else. AB_Als (set in the
	// constructor) re-initializes on the zombie mesh because UE4_Mannequin_Skeleton lists
	// SK_Als as a compatible skeleton (emerge_py/compat_skeletons.py, saved into both skeleton
	// assets). ALS keeps owning rotation, gait, tick options, mantling — the zombie simply plays
	// the human ALS anims until the zombie clip layer lands.
	MeshComp->SetSkeletalMeshAsset(Zombie);
	bZombieLookActive = true;
	UE_LOG(LogTemp, Warning,
		TEXT("EMERGE_ZLOOK: %s: mesh swapped OK (mesh=%s skeleton=%s), ALS pipeline retained (animClass=%s)"),
		*GetName(), *Zombie->GetName(),
		Zombie->GetSkeleton() ? *Zombie->GetSkeleton()->GetName() : TEXT("NULL"),
		MeshComp->GetAnimClass() ? *MeshComp->GetAnimClass()->GetName() : TEXT("NULL"));
}

UAlsMantlingSettings* AEmergeEnemy::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	// Zombie mantle variety: roll a random speed variant PER MANTLE (that roll IS the variance —
	// the 3 variants are the same ALS montage rate-scaled to 0.50/0.60/0.72). Non-zombie (or an
	// empty bank, i.e. make_zombie_mantles.py not run yet) falls back to the original single
	// player-speed settings.
	const TArray<TObjectPtr<UAlsMantlingSettings>>& Bank =
		(MantlingType == EAlsMantlingType::Low) ? ZombieMantleLow : ZombieMantleHigh;
	if (bZombieLookActive && Bank.Num() > 0)
	{
		return Bank[FMath::RandRange(0, Bank.Num() - 1)];
	}
	return (MantlingType == EAlsMantlingType::Low) ? MantlingSettingsLow : MantlingSettingsHigh;
}

FString AEmergeEnemy::GetAnimDebug() const
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	const UAnimInstance* Anim = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	return FString::Printf(TEXT("{\"anim\":%s,\"gait\":\"%s\",\"action\":\"%s\",\"spd\":%.0f,\"zombieLook\":%s}"),
		Anim ? *FString::Printf(TEXT("\"%s\""), *Anim->GetClass()->GetName()) : TEXT("null"),
		*GetDesiredGait().GetTagName().ToString(),
		*GetLocomotionAction().GetTagName().ToString(), GetVelocity().Size2D(),
		bZombieLookActive ? TEXT("true") : TEXT("false"));
}
