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

// Base survivor pawn for the first playable slice. Ships with the full core survival runtime suite
// attached (vitals, stagger, stamina, damage resolver, status effects, equipment) so the tested gameplay
// math is live on the pawn out of the box. Movement/input/mesh are wired in the editor/Blueprint child.
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
};
