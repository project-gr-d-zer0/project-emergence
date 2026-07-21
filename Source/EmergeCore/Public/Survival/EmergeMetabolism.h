#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMetabolism.generated.h"

UCLASS()
class EMERGECORE_API UEmergeMetabolism : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Metabolism")
	static float MetabolicSpeed(int32 Movement);

	UFUNCTION(BlueprintCallable, Category = "Survival|Metabolism")
	static float ConsumptionModifier(float Stat);

	UFUNCTION(BlueprintCallable, Category = "Survival|Metabolism")
	static float DrainPerTick(int32 Movement, float Stat, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Metabolism")
	static bool IsLow(float Stat);

	UFUNCTION(BlueprintCallable, Category = "Survival|Metabolism")
	static float LowStatHealthDamage(float Stat, float DeltaT);
};