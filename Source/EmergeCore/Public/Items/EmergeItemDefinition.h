#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmergeItemDefinition.generated.h"

// Data-driven item definition (spec: "DayZ inventory, modernized" + Tarkov-style grid footprint).
// Items are content authored as assets; systems reference definitions, never hard-code item data.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	// Per-unit weight in kilograms.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0.0"))
	float WeightKg = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 MaxStack = 1;

	// Grid footprint in cells (width x height), Tarkov/DayZ-style container packing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 GridWidth = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 GridHeight = 1;

	// New Property for Max Durability
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0.0"))
	float MaxDurability = 1.0f;
};