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

private:
	UPROPERTY()
	TArray<FEmergeInventoryEntry> Entries;
};
