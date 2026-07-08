#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EmergeCharacter.generated.h"

class UEmergeVitalsComponent;
class UEmergeStaggerComponent;
class UEmergeStaminaComponent;
class UEmergeDamageComponent;
class UEmergeStatusEffectComponent;
class UEmergeEquipmentComponent;

// Base survivor pawn. Ships with the full core survival runtime suite attached (vitals, stagger,
// stamina, damage resolver, status effects, equipment) so the tested gameplay math is live on the
// pawn out of the box. Basic WASD+mouse+jump input is bound in C++ (axis mappings in
// Config/DefaultInput.ini); locomotion speed is scaled every frame by the stagger state.
UCLASS()
class EMERGECORE_API AEmergeCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AEmergeCharacter();

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

	// Walk speed at stagger multiplier 1.0; the stagger state scales it down each frame.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Movement")
	float BaseWalkSpeed = 500.0f;

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnYaw(float Value);
	void LookUpPitch(float Value);
};
