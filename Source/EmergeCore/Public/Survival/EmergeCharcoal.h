#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCharcoal.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCharcoal : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Charcoal")
	static float FoodPoisonDieOffPerSec(bool bCharcoalActive);

	UFUNCTION(BlueprintCallable, Category = "Survival|Charcoal")
	static float AgentReducedPerTick(bool bCharcoalActive, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Charcoal")
	static float TimeToClear(float AgentCount, bool bCharcoalActive);
};