#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeInventorySort.generated.h"

UCLASS()
class EMERGECORE_API UEmergeInventorySort : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Sorting")
	static int32 CompareByWeight(float WeightA, float WeightB);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Sorting")
	static int32 CompareByValue(int32 ValueA, int32 ValueB);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Sorting")
	static bool ShouldSwap(float KeyA, float KeyB, bool bAscending);
};