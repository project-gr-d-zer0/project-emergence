#include "Framework/EmergeCharacter.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"
#include "Survival/EmergeStaminaComponent.h"
#include "Combat/EmergeDamageComponent.h"
#include "Survival/EmergeStatusEffectComponent.h"
#include "Combat/EmergeEquipmentComponent.h"
#include "Inventory/EmergeInventoryComponent.h"
#include "Combat/EmergeStagger.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Utility/AlsGameplayTags.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"

AEmergeCharacter::AEmergeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Vitals = CreateDefaultSubobject<UEmergeVitalsComponent>(TEXT("Vitals"));
	Stagger = CreateDefaultSubobject<UEmergeStaggerComponent>(TEXT("Stagger"));
	Stamina = CreateDefaultSubobject<UEmergeStaminaComponent>(TEXT("Stamina"));
	Damage = CreateDefaultSubobject<UEmergeDamageComponent>(TEXT("Damage"));
	StatusEffects = CreateDefaultSubobject<UEmergeStatusEffectComponent>(TEXT("StatusEffects"));
	Equipment = CreateDefaultSubobject<UEmergeEquipmentComponent>(TEXT("Equipment"));
	Inventory = CreateDefaultSubobject<UEmergeInventoryComponent>(TEXT("Inventory"));

	// ALS gameplay + movement settings (required or the animation refresh early-returns).
	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> AlsSettings(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (AlsSettings.Succeeded()) { Settings = AlsSettings.Object; }
	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> AlsMovement(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (AlsMovement.Succeeded()) { MovementSettings = AlsMovement.Object; }

	// Enhanced Input: assign the ALS mapping context + action assets (inherited protected members).
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(
		TEXT("/ALS/ALS/Data/Input/IMC_Als_Default.IMC_Als_Default"));
	if (IMC.Succeeded()) { InputMappingContext = IMC.Object; }

#define EMERGE_FIND_IA(Member, Path) \
	{ static ConstructorHelpers::FObjectFinder<UInputAction> Finder_##Member(TEXT(Path)); \
	  if (Finder_##Member.Succeeded()) { Member = Finder_##Member.Object; } }

	EMERGE_FIND_IA(LookMouseAction,     "/ALS/ALS/Data/Input/IA_Als_LookMouse.IA_Als_LookMouse")
	EMERGE_FIND_IA(LookAction,          "/ALS/ALS/Data/Input/IA_Als_Look.IA_Als_Look")
	EMERGE_FIND_IA(MoveAction,          "/ALS/ALS/Data/Input/IA_Als_Move.IA_Als_Move")
	EMERGE_FIND_IA(SprintAction,        "/ALS/ALS/Data/Input/IA_Als_Sprint.IA_Als_Sprint")
	EMERGE_FIND_IA(WalkAction,          "/ALS/ALS/Data/Input/IA_Als_Walk.IA_Als_Walk")
	EMERGE_FIND_IA(CrouchAction,        "/ALS/ALS/Data/Input/IA_Als_Crouch.IA_Als_Crouch")
	EMERGE_FIND_IA(JumpAction,          "/ALS/ALS/Data/Input/IA_Als_Jump.IA_Als_Jump")
	EMERGE_FIND_IA(AimAction,           "/ALS/ALS/Data/Input/IA_Als_Aim.IA_Als_Aim")
	EMERGE_FIND_IA(RagdollAction,       "/ALS/ALS/Data/Input/IA_Als_Ragdoll.IA_Als_Ragdoll")
	EMERGE_FIND_IA(RollAction,          "/ALS/ALS/Data/Input/IA_Als_Roll.IA_Als_Roll")
	EMERGE_FIND_IA(RotationModeAction,  "/ALS/ALS/Data/Input/IA_Als_RotationMode.IA_Als_RotationMode")
	EMERGE_FIND_IA(ViewModeAction,      "/ALS/ALS/Data/Input/IA_Als_ViewMode.IA_Als_ViewMode")
	EMERGE_FIND_IA(SwitchShoulderAction,"/ALS/ALS/Data/Input/IA_Als_SwitchShoulder.IA_Als_SwitchShoulder")

#undef EMERGE_FIND_IA
}

void AEmergeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);   // ALS example: camera + input-driven locomotion

	// Stagger differentiator: a knockdown drops the survivor into a physics ragdoll.
	if (Stagger)
	{
		const EEmergeStaggerState State = Stagger->CurrentState();
		if ((State == EEmergeStaggerState::Knockdown || State == EEmergeStaggerState::Dead) &&
			GetLocomotionAction() != AlsLocomotionActionTags::Ragdolling)
		{
			StartRagdolling();
		}
	}

	// Bleeding drains vitals continuously.
	if (StatusEffects && Vitals)
	{
		StatusEffects->Simulate(DeltaSeconds, Vitals);
	}
}
