#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeIntercept.generated.h"

// Deterministic lead-pursuit helpers so a chaser intercepts a moving target rather than tail-chasing.
UCLASS()
class EMERGECORE_API UEmergeIntercept : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Nav|Intercept")
	static FVector PredictPosition(FVector TargetPos, FVector TargetVel, float LeadTime);

	UFUNCTION(BlueprintPure, Category = "Nav|Intercept")
	static float LeadTime(float Distance, float PursuerSpeed);
};
