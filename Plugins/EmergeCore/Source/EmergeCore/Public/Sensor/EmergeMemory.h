#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMemory.generated.h"

// Target/world memory. Two layers: a smooth confidence decay (awareness UI) AND the PZ-grounded discrete
// forget-timeout that actually governs when a zombie drops its target. [CORPUS-FIX] PZ IsoZombie.java: memory
// tiers 1250/800/500/25 (High/Normal/Low/None); TimeSinceSeenFlesh accumulates ~GameTime.getMultiplier()/update,
// and once it exceeds the tier the target is dropped instantly.
UCLASS()
class EMERGECORE_API UEmergeMemory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Smooth confidence decay (awareness UI), clamped [0,1].
	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static float ConfidenceAfter(float LastConf, float DtSeconds, float DecaySeconds);

	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static bool IsStale(float Confidence, float Floor);

	// PZ forget-timeout: the actual "drop the target" behavior. MemoryTier 0 None,1 Low,2 Normal,3 High.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static float ForgetThreshold(int32 MemoryTier);

	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static float AccumulateTimeSinceSeen(float Current, float UpdateMultiplier);

	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static bool HasForgotten(float TimeSinceSeen, int32 MemoryTier);
};
