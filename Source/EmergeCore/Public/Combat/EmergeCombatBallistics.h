#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCombatBallistics.generated.h"

// Range-based ballistics falloff (distinct concept from UEmergeBallistics' start/end damage
// falloff): a normalized 0..1 factor over a Min/Max range band, with a degenerate-range guard.
UCLASS()
class EMERGECORE_API UEmergeCombatBallistics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Linear falloff factor across the effective range band:
	//   factor = clamp((MaxRange - InDistance) / (MaxRange - MinRange), 0, 1)
	// Degenerate guard: MaxRange <= MinRange returns exactly 1.0f (no division by zero / NaN).
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static float ComputeRangeFalloff(float InDistance, float MinRange, float MaxRange);
};
