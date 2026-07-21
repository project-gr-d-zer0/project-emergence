#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFireSpread.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFireSpread : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|FireSpread")
	static float SpreadChance(int32 Weather, bool bDryFuel);

	UFUNCTION(BlueprintCallable, Category = "Survival|FireSpread")
	static bool Spreads(float Roll01, float Chance);
};