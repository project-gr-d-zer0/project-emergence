#pragma once
#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "EmergeCharacter.generated.h"

class UEmergeVitalsComponent;
class UEmergeStaggerComponent;
class UEmergeStaminaComponent;
class UEmergeDamageComponent;
class UEmergeStatusEffectComponent;
class UEmergeEquipmentComponent;
class UEmergeInventoryComponent;

// Base survivor pawn on the ALS-Refactored locomotion spine (replicated gaits/stances/mantling/ragdoll).
// Ships with the full core survival runtime suite attached, and bridges the tested gameplay math into
// ALS every frame: stagger state + sprint intent + stamina -> mobility tier -> ALS gait / ragdoll.
// Knockdown = real ragdoll (stumble differentiator, phase-3 foundation); encumbrance scales sprint drain.
UCLASS()
class EMERGECORE_API AEmergeCharacter : public AAlsCharacter
{
	GENERATED_BODY()
public:
	explicit AEmergeCharacter(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeVitalsComponent> Vitals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeStaggerComponent> Stagger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeStaminaComponent> Stamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeDamageComponent> Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeStatusEffectComponent> StatusEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeEquipmentComponent> Equipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge")
	TObjectPtr<UEmergeInventoryComponent> Inventory;

	// Sprint intent from input; actual sprinting is gated by stagger state + stamina in the bridge.
	UPROPERTY(BlueprintReadOnly, Category = "Emerge|Movement")
	bool bWantsToSprint = false;

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnYaw(float Value);
	void LookUpPitch(float Value);
	void SprintPressed();
	void SprintReleased();
	void ToggleCrouch();
	void AimPressed();
	void AimReleased();

	// Local crouch toggle state (ALS stance is driven by SetDesiredStance).
	bool bCrouched = false;
};
