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

// Base survivor pawn on the ALS-Refactored locomotion spine (replicated gaits/stances/mantling/ragdoll).
// Ships with the full core survival runtime suite attached (vitals, stagger, stamina, damage resolver,
// status effects, equipment) so the tested gameplay math is live on the pawn out of the box.
// Basic WASD+mouse+jump input is bound in C++ (axis mappings in Config/DefaultInput.ini).
// TODO(next): map stagger states onto ALS - Knockdown -> StartRagdolling(), Stumble/Stagger -> forced walk gait.
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

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnYaw(float Value);
	void LookUpPitch(float Value);
};
