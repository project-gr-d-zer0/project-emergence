#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeVitals.generated.h"

// Deterministic survival vitals math (hydration/health depletion + starvation).
// Pure, data-driven, unit-testable headless. No Tick, no rendering, no networking.
UCLASS()
class EMERGECORE_API UEmergeVitals : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Depletes Current by RatePerSec * DeltaSeconds, clamped to [0, 100].
	UFUNCTION(BlueprintCallable, Category = "Survival|Vitals")
	static float Deplete(float Current, float RatePerSec, float DeltaSeconds);

	// True when hydration has reached or gone below zero (starving).
	UFUNCTION(BlueprintPure, Category = "Survival|Vitals")
	static bool IsStarving(float Hydration);

	// Health damage applied while starving; zero otherwise.
	UFUNCTION(BlueprintCallable, Category = "Survival|Vitals")
	static float StarvationDamage(float Hydration, float DeltaSeconds, float DamagePerSec);

	// Fall damage from vertical impact speed (cm/s). Safe below 800 cm/s; scales linearly
	// above that and caps at a lethal 100.
	UFUNCTION(BlueprintCallable, Category = "Survival|Vitals")
	static float FallDamage(float ImpactSpeed);
};
