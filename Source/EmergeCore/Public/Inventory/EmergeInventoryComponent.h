#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeInventoryComponent.generated.h"

class UEmergeItemDefinition;

USTRUCT(BlueprintType)
struct FEmergeInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UEmergeItemDefinition> Item = nullptr;

	UPROPERTY()
	int32 Quantity = 0;
};

// Weight-gated inventory. Deterministic + renderless so it is fully unit-testable headless.
UCLASS(ClassGroup = (Emergence), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeInventoryComponent();

	// Maximum carry weight in kilograms.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0.0"))
	float MaxWeightKg = 30.0f;

	// Encumbrance band thresholds, expressed as a fraction of MaxWeightKg.
	// ratio <= LightMaxFraction            -> tier 0 (Light)
	// LightMaxFraction < ratio <= HeavyMinFraction -> tier 1 (Normal)
	// ratio > HeavyMinFraction              -> tier 2 (Heavy)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encumbrance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LightMaxFraction = 0.50f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encumbrance", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HeavyMinFraction = 0.75f;

	// Adds the full quantity, or nothing if it would exceed MaxWeightKg. Returns true on success.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItem(UEmergeItemDefinition* Item, int32 Quantity = 1);

	// Removes up to Quantity; returns the number actually removed.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(UEmergeItemDefinition* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentWeightKg() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetQuantity(const UEmergeItemDefinition* Item) const;

	// Ratio of current weight to MaxWeightKg (0 when MaxWeightKg <= 0).
	UFUNCTION(BlueprintPure, Category = "Encumbrance")
	float GetLoadRatio() const;

	// Encumbrance tier: 0 = Light (ratio <= LightMaxFraction), 1 = Normal, 2 = Heavy (ratio > HeavyMinFraction).
	UFUNCTION(BlueprintPure, Category = "Encumbrance")
	int32 GetLoadTier() const;

private:
	UPROPERTY()
	TArray<FEmergeInventoryEntry> Entries;
};
