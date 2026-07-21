#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDisease.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDisease : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static const int32 GREAT = 0;
	static const int32 HIGH = 1;
	static const int32 MEDIUM = 2;
	static const int32 LOW = 3;
	static const int32 CRITICAL = 4;

	UFUNCTION(BlueprintCallable, Category = "Disease")
	static float Immunity01(float Energy, float Water, float Health01, float Blood01);

	UFUNCTION(BlueprintCallable, Category = "Disease")
	static int32 ImmunityLevel(float Immunity01);

	UFUNCTION(BlueprintCallable, Category = "Disease")
	static bool AgentGrows(int32 AgentPotency, int32 ImmunityLevel);

	UFUNCTION(BlueprintCallable, Category = "Disease")
	static float GrowDelta(float Invasibility, float DieOffSpeed, int32 AgentPotency, int32 ImmunityLevel, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Disease")
	static float StepCount(float OldCount, float GrowDelta, float MaxCount);
};