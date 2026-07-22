#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeTemperature.generated.h"

// Body temperature drift for DayZ-style survival: cold/wet chills, heat sources warm.
// Comfort band centers on 37 C; out-of-band feeds health/status drains (wired later).
UCLASS()
class EMERGECORE_API UEmergeTemperature : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Body temperature delta per second given ambient temp (C), wetness [0,1], and nearby heat source.
	UFUNCTION(BlueprintCallable, Category = "Survival|Temperature")
	static float BodyTempDeltaPerSec(float AmbientC, float Wetness01, bool bNearHeatSource);

	// True when body temperature is below the hypothermia threshold (35 C).
	UFUNCTION(BlueprintCallable, Category = "Survival|Temperature")
	static bool IsHypothermic(float BodyTempC);

	// True when body temperature is above the hyperthermia threshold (39 C).
	UFUNCTION(BlueprintCallable, Category = "Survival|Temperature")
	static bool IsHyperthermic(float BodyTempC);
};
