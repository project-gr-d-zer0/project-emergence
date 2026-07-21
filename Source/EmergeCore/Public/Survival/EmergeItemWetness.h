#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeItemWetness.generated.h"

UCLASS()
class EMERGECORE_API UEmergeItemWetness : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Item Wetness")
	static float WeightMultiplier(int32 WetnessState);

	UFUNCTION(BlueprintCallable, Category = "Survival|Item Wetness")
	static float WettingRatePerTick(bool bInLiquid, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Item Wetness")
	static float DryingRatePerTick(bool bOnGround, float DeltaT);
};