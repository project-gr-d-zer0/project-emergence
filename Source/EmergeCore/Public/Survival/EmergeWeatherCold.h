#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWeatherCold.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWeatherCold : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Weather")
	static float ColdRatePerTick(int32 Weather, bool bSheltered, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Weather")
	static bool IsFreezingWeather(int32 Weather);
};