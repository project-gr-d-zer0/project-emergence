#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWeightHud.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWeightHud : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|WeightHud")
	static float PenalizedWeightKg(float WeightGrams);

	UFUNCTION(BlueprintCallable, Category = "Shell|WeightHud")
	static float StaminaPenalty(float WeightGrams);

	UFUNCTION(BlueprintCallable, Category = "Shell|WeightHud")
	static float MaxStaminaForWeight(float WeightGrams);

	UFUNCTION(BlueprintCallable, Category = "Shell|WeightHud")
	static bool IsOverloaded(float WeightGrams);
};