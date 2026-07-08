#include "Framework/EmergeCharacter.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"
#include "Survival/EmergeStaminaComponent.h"
#include "Combat/EmergeDamageComponent.h"
#include "Survival/EmergeStatusEffectComponent.h"
#include "Combat/EmergeEquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

AEmergeCharacter::AEmergeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;

	Vitals = CreateDefaultSubobject<UEmergeVitalsComponent>(TEXT("Vitals"));
	Stagger = CreateDefaultSubobject<UEmergeStaggerComponent>(TEXT("Stagger"));
	Stamina = CreateDefaultSubobject<UEmergeStaminaComponent>(TEXT("Stamina"));
	Damage = CreateDefaultSubobject<UEmergeDamageComponent>(TEXT("Damage"));
	StatusEffects = CreateDefaultSubobject<UEmergeStatusEffectComponent>(TEXT("StatusEffects"));
	Equipment = CreateDefaultSubobject<UEmergeEquipmentComponent>(TEXT("Equipment"));
}

void AEmergeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// The stumble differentiator, live: stagger state directly gates locomotion speed.
	if (Stagger && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * Stagger->SpeedMultiplier();
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
}

void AEmergeCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEmergeCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
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
