#include "Framework/EmergeCharacter.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"
#include "Survival/EmergeStaminaComponent.h"
#include "Combat/EmergeDamageComponent.h"
#include "Survival/EmergeStatusEffectComponent.h"
#include "Combat/EmergeEquipmentComponent.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "UObject/ConstructorHelpers.h"
#include "Inventory/EmergeInventoryComponent.h"
#include "Combat/EmergeStagger.h"
#include "Utility/AlsGameplayTags.h"
#include "Components/InputComponent.h"

AEmergeCharacter::AEmergeCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Vitals = CreateDefaultSubobject<UEmergeVitalsComponent>(TEXT("Vitals"));
	Stagger = CreateDefaultSubobject<UEmergeStaggerComponent>(TEXT("Stagger"));
	Stamina = CreateDefaultSubobject<UEmergeStaminaComponent>(TEXT("Stamina"));
	Damage = CreateDefaultSubobject<UEmergeDamageComponent>(TEXT("Damage"));
	StatusEffects = CreateDefaultSubobject<UEmergeStatusEffectComponent>(TEXT("StatusEffects"));
	Equipment = CreateDefaultSubobject<UEmergeEquipmentComponent>(TEXT("Equipment"));
	Inventory = CreateDefaultSubobject<UEmergeInventoryComponent>(TEXT("Inventory"));

	// ALS gameplay + movement settings. AAlsCharacter guards its animation refresh on IsValid(Settings)
	// (AlsCharacter.cpp ~L271), so without these the pose never updates while moving. ALS ships no
	// defaults for them (B_Als_Character assigns them in BP); assign them here so every survivor works.
	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> AlsSettings(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (AlsSettings.Succeeded())
	{
		Settings = AlsSettings.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> AlsMovement(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (AlsMovement.Succeeded())
	{
		MovementSettings = AlsMovement.Object;
	}
}

void AEmergeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!Stagger || !Stamina)
	{
		return;
	}

	// The tested mobility mapping: stagger state + sprint intent + stamina -> 0 ragdoll / 1 walk / 2 run / 3 sprint.
	const int32 Mobility = UEmergeStagger::MobilityForState(Stagger->CurrentState(), bWantsToSprint, Stamina->CanSprint());
	const bool bRagdolling = GetLocomotionAction() == AlsLocomotionActionTags::Ragdolling;

	if (Mobility == 0)
	{
		if (!bRagdolling)
		{
			StartRagdolling();
		}
	}
	else
	{
		if (bRagdolling)
		{
			StopRagdolling();
		}
		SetDesiredGait(Mobility >= 3 ? AlsGaitTags::Sprinting
			: Mobility == 2 ? AlsGaitTags::Running
			: AlsGaitTags::Walking);
	}

	// Stamina: sprint drains (scaled by encumbrance load tier), otherwise regenerates.
	const int32 LoadTier = Inventory ? Inventory->GetLoadTier() : 0;
	Stamina->Simulate(DeltaSeconds, Mobility >= 3, LoadTier);

	// Status effects (bleeding) drain vitals continuously.
	if (StatusEffects && Vitals)
	{
		StatusEffects->Simulate(DeltaSeconds, Vitals);
	}
}

void AEmergeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AEmergeCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AEmergeCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AEmergeCharacter::TurnYaw);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AEmergeCharacter::LookUpPitch);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AEmergeCharacter::SprintPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AEmergeCharacter::SprintReleased);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AEmergeCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &AEmergeCharacter::AimPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &AEmergeCharacter::AimReleased);
}

void AEmergeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// ALS rotates the character itself; move relative to the control rotation's yaw.
		const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void AEmergeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

void AEmergeCharacter::TurnYaw(float Value)
{
	AddControllerYawInput(Value);
}

void AEmergeCharacter::LookUpPitch(float Value)
{
	AddControllerPitchInput(Value);
}

void AEmergeCharacter::SprintPressed()
{
	bWantsToSprint = true;
}

void AEmergeCharacter::SprintReleased()
{
	bWantsToSprint = false;
}

void AEmergeCharacter::ToggleCrouch()
{
	bCrouched = !bCrouched;
	SetDesiredStance(bCrouched ? AlsStanceTags::Crouching : AlsStanceTags::Standing);
}

void AEmergeCharacter::AimPressed()
{
	SetDesiredAiming(true);
	SetDesiredRotationMode(AlsRotationModeTags::Aiming);
}

void AEmergeCharacter::AimReleased()
{
	SetDesiredAiming(false);
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
}
