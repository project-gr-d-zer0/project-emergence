#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWaterPurity.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWaterPurity : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|WaterPurity")
	static float ContaminationChance(int32 Source);

	UFUNCTION(BlueprintCallable, Category = "Survival|WaterPurity")
	static bool IsSafe(int32 Source);

	UFUNCTION(BlueprintCallable, Category = "Survival|WaterPurity")
	static float PurifiedChance(float BaseChance, bool bPurified);
};