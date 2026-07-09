#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMemory.generated.h"

// Persistent world-memory confidence decay (entities/places age out like a forgetting curve).
// Linear decay for exact determinism. Deterministic, data-driven-friendly, unit-testable headless.
UCLASS()
class EMERGECORE_API UEmergeMemory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Linear decay of confidence over DtSeconds given a DecaySeconds constant, clamped to [0,1].
	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static float ConfidenceAfter(float LastConf, float DtSeconds, float DecaySeconds);

	// True when Confidence has decayed below Floor.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Memory")
	static bool IsStale(float Confidence, float Floor);
};
