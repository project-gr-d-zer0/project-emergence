#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHeatSource.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHeatSource : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Emergence|Survival")
	static float HeatAtDistance(float SourceHeat, float Distance, float MaxRange);

	UFUNCTION(BlueprintCallable, Category = "Emergence|Survival")
	static bool InWarmthRange(float Distance, float MaxRange);

	UFUNCTION(BlueprintCallable, Category = "Emergence|Survival")
	static float FireplaceHeatingRate(float BaseRate);
};