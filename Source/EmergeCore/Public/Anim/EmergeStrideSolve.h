#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStrideSolve.generated.h"

// Play-rate + stride-scale split solver (research: Lyra-style foot-slide fix).
// Rate is clamped 1.0..1.2; stride scale covers whatever speed remains after the rate contribution.
UCLASS()
class EMERGECORE_API UEmergeStrideSolve : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Anim|Stride")
	static float PlayRate(float SpeedUu, float AuthoredSpeedUu);

	UFUNCTION(BlueprintCallable, Category = "Anim|Stride")
	static float StrideScale(float SpeedUu, float AuthoredSpeedUu, float InPlayRate);
};
