#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAdsTime.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAdsTime : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|ADS")
	static float AdsTimeSec(float BaseTime, float Ergonomics01, float WeightPenalty01);
};