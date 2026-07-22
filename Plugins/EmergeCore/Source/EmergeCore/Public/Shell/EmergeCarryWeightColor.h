#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCarryWeightColor.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCarryWeightColor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|CarryWeightColor")
	static float WeightFraction01(float Current, float Max);

	UFUNCTION(BlueprintCallable, Category = "Shell|CarryWeightColor")
	static int32 WeightColorTier(float Fraction01);
};