#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNoise.generated.h"

// How a listener responds to a heard noise.
UENUM(BlueprintType)
enum class EEmergeNoiseResponse : uint8
{
	Ignore,        // did not hear it
	Investigate,   // heard something; moves to last-known position, decays
	TargetExact    // localized the source exactly; direct aggro
};

// Deterministic noise emission/propagation/detection math — the layer that makes suppressors, melee,
// and stealth meaningful, and turns depth-density into real pressure (unsuppressed fire deep = suicide).
// Unsuppressed gunfire is exactly localized by everything in radius; suppressed/quiet noises are only
// exactly localized by line-of-sight witnesses, others just investigate.
UCLASS()
class EMERGECORE_API UEmergeNoise : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Effective audible radius: BaseRadius * EnvMult (rain/indoors < 1) * DepthMult, floored at 0.
	UFUNCTION(BlueprintCallable, Category = "World|Noise")
	static float EffectiveRadius(float BaseRadius, float EnvMult, float DepthMult);

	// A listener at DistanceM hears the noise when 0 <= DistanceM <= EffectiveRadiusM.
	UFUNCTION(BlueprintCallable, Category = "World|Noise")
	static bool IsHeard(float EffectiveRadiusM, float DistanceM);

	// Horde attraction weight for a heard noise: Volume / max(1, DistanceM). Louder + closer pulls harder.
	UFUNCTION(BlueprintCallable, Category = "World|Noise")
	static float AttractionWeight(float Volume, float DistanceM);

	// Response rules: not heard -> Ignore; unsuppressed heard -> TargetExact;
	// suppressed heard -> TargetExact only with line of sight, else Investigate.
	UFUNCTION(BlueprintCallable, Category = "World|Noise")
	static EEmergeNoiseResponse ResponseToShot(bool bSuppressed, bool bHeard, bool bLineOfSight);
};
