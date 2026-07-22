#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCornering.generated.h"

// Cornering speed scale — pursuers slow while turning hard (Dying Light Volatile pattern).
// Deterministic, data-driven escape margin: straight-line chase speed stays honest, but
// juking/corners open the gap because the AI must shed speed to turn sharply.
UCLASS()
class EMERGECORE_API UEmergeCornering : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Returns a [MinScale..1.0] multiplier based on absolute turn error (degrees):
	// 1.0 at/below StartDeg, linearly falls to MinScale by FullDeg, clamped beyond.
	UFUNCTION(BlueprintCallable, Category = "Nav|Cornering")
	static float SpeedScale(float TurnErrorDeg, float StartDeg, float FullDeg, float MinScale);
};
