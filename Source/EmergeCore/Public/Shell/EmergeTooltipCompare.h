#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeTooltipCompare.generated.h"

UCLASS()
class EMERGECORE_API UEmergeTooltipCompare : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|TooltipCompare")
	static int32 CompareStat(float NewVal, float CurrentVal, bool bHigherBetter);

	UFUNCTION(BlueprintCallable, Category = "Shell|TooltipCompare")
	static float StatDelta(float NewVal, float CurrentVal);
};