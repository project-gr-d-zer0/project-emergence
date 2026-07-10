#include "AI/EmergeEnemy.h"
#include "AI/EmergeEnemyController.h"
#include "Components/SkeletalMeshComponent.h"
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
	// Zombie speeds: duplicate MS_Als_Normal (keeps the accel/rotation curves) and override gait
	// speeds — Walking = shamble 150, Running = chase 560 (research-tuned vs player run 375/sprint 640).
	// Must happen BEFORE Super, which hands MovementSettings to the movement component.
	if (BaseMovementSettings)
	{
		UAlsMovementSettings* Zombie = DuplicateObject(BaseMovementSettings.Get(), this, TEXT("ZombieMovementSettings"));
		for (TPair<FGameplayTag, FAlsMovementStanceSettings>& RotationPair : Zombie->RotationModes)
		{
			for (TPair<FGameplayTag, FAlsMovementGaitSettings>& StancePair : RotationPair.Value.Stances)
			{
				StancePair.Value.WalkForwardSpeed = 150.0f;
				StancePair.Value.WalkBackwardSpeed = 150.0f;
				StancePair.Value.RunForwardSpeed = 560.0f;
				StancePair.Value.RunBackwardSpeed = 560.0f;
				StancePair.Value.SprintSpeed = 650.0f;
			}
		}
		MovementSettings = Zombie;
	}
	Super::PostInitializeComponents();
}

void AEmergeEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Face travel direction (an AI has no meaningful view direction) and start at shamble.
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
	SetDesiredGait(AlsGaitTags::Walking);
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
