#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSpreadStance.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSpreadStance : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Spread")
	static float StanceMultiplier(int32 Stance);

	UFUNCTION(BlueprintCallable, Category = "Combat|Spread")
	static float EffectiveSpreadMoa(float BaseMoa, int32 Stance, bool bMoving, bool bAds);
};