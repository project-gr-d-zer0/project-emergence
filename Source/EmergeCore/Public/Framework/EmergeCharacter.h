#pragma once
#include "CoreMinimal.h"
#include "AlsCharacterExample.h"
#include "EmergeCharacter.generated.h"

EMERGECORE_API DECLARE_LOG_CATEGORY_EXTERN(LogEmergeTelemetry, Log, All);

class UEmergeVitalsComponent;
class UEmergeStaggerComponent;
class UEmergeStaminaComponent;
class UEmergeDamageComponent;
class UEmergeStatusEffectComponent;
class UEmergeEquipmentComponent;
class UEmergeInventoryComponent;

// Survivor pawn built on ALS's example character: inherits its complete, proven Enhanced Input
// (move/look/sprint/crouch/jump/aim/mantle/ragdoll) + ALS camera. Adds the full survival runtime suite
// and a minimal stagger->ragdoll bridge (knockdown = ragdoll; bleeding drains vitals).
UCLASS()
class EMERGECORE_API AEmergeCharacter : public AAlsCharacterExample
{
	GENERATED_BODY()
public:
	AEmergeCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeVitalsComponent> Vitals;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeStaggerComponent> Stagger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeStaminaComponent> Stamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeDamageComponent> Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeStatusEffectComponent> StatusEffects;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeEquipmentComponent> Equipment;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeInventoryComponent> Inventory;

	virtual void Tick(float DeltaSeconds) override;
};
