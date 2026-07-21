#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWaterFreeze.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWaterFreeze : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|WaterFreeze")
	static bool IsFrozen(float TempC);

	UFUNCTION(BlueprintCallable, Category = "Survival|WaterFreeze")
	static bool CanDrink(bool bFrozen);

	UFUNCTION(BlueprintCallable, Category = "Survival|WaterFreeze")
	static float MeltRatePerTick(float AmbientC, float DeltaT);
};