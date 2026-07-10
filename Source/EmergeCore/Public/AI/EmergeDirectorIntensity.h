#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDirectorIntensity.generated.h"

// L4D-style AI Director intensity math (Booth AIIDE-09): intensity from damage taken + nearby deaths,
// with decay frozen while enemies are engaged. Pure/static so it is deterministic and unit-testable headless.
UCLASS()
class EMERGECORE_API UEmergeDirectorIntensity : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Intensity contribution from damage taken, clamped to 0..1.
	UFUNCTION(BlueprintCallable, Category = "Emerge|AI|Director")
	static float IntensityFromDamage(float DamageFraction);

	// Intensity contribution from a nearby death, inverse distance falloff capped at 0.3.
	UFUNCTION(BlueprintCallable, Category = "Emerge|AI|Director")
	static float IntensityFromNearbyDeath(float DistUu);

	// Decays Current toward 0 with a 10s half-life, but is frozen (unchanged) while enemies are engaged.
	UFUNCTION(BlueprintCallable, Category = "Emerge|AI|Director")
	static float Decay(float Current, float Dt, bool bEnemiesEngaged);
};
