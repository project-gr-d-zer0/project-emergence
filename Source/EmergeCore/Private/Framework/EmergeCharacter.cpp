#include "Framework/EmergeCharacter.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"
#include "Survival/EmergeStaminaComponent.h"
#include "Combat/EmergeDamageComponent.h"
#include "Survival/EmergeStatusEffectComponent.h"
#include "Combat/EmergeEquipmentComponent.h"
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
