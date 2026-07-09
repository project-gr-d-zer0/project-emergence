#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAwareness.generated.h"

// Gradual detection ("slow identify"): awareness fills over time (faster when close + LOS),
// decays when unseen. Deterministic, data-driven math only.
UCLASS()
class EMERGECORE_API UEmergeAwareness : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Fill rate per second: 0 without LOS or out of range; closer -> faster.
	UFUNCTION(BlueprintCallable, Category = "Nav|Awareness")
	static float FillRate(float Dist, float MaxDist, bool bHasLineOfSight);

	// Accumulates Current by FillRate * DtSeconds, clamped to [0,1].
	UFUNCTION(BlueprintCallable, Category = "Nav|Awareness")
	static float Accumulate(float Current, float FillRate, float DtSeconds);

	// Decays Current by DecayPerSecond * DtSeconds, clamped to [0,1].
	UFUNCTION(BlueprintCallable, Category = "Nav|Awareness")
	static float Decay(float Current, float DecayPerSecond, float DtSeconds);

	// Maps awareness [0,1] to a discrete state: 0=Unaware,1=Suspicious,2=Alerted,3=Chasing.
	UFUNCTION(BlueprintCallable, Category = "Nav|Awareness")
	static int32 StateFor(float Awareness);
};
