#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeWetness.generated.h"

UCLASS(BlueprintType)
class EMERGECORE_API UEmergeWetness : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival")
	static float WetDeltaPerTick(bool bRaining, bool bInBackpack, float WindStrength01);

	UFUNCTION(BlueprintPure, Category = "Survival")
	static bool IsSoakingWet(float Wetness);

	UFUNCTION(BlueprintPure, Category = "Survival")
	static bool WetPenaltyActive(float Wetness);

	UFUNCTION(BlueprintPure, Category = "Survival")
	static float SoakedWeightMultiplier(bool bSoaked);
};