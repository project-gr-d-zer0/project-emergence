#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeItemInstance.generated.h"

// Lightweight, renderless item instance query surface. Kept a plain UObject for
// deterministic, headless unit-testability (no actor/component overhead).
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeItemInstance : public UObject
{
	GENERATED_BODY()
public:
	// An item is broken when its current durability has been fully depleted (<= 0).
	UFUNCTION(BlueprintPure, Category = "Item")
	static bool IsBroken(float CurrentDurability);
};
