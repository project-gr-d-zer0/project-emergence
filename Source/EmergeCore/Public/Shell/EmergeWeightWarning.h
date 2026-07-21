#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWeightWarning.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWeightWarning : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|WeightWarning")
	static int32 WarningLevel(float CurrentKg, float MaxKg);

	UFUNCTION(BlueprintCallable, Category = "Shell|WeightWarning")
	static bool IsOverloaded(float CurrentKg, float MaxKg);
};