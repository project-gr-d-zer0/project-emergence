#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBloodRegen.generated.h"

// Deterministic blood regeneration math (gated by hydration and energy).
// Pure, data-driven, unit-testable headless. No Tick, no rendering, no networking.
UCLASS()
class EMERGECORE_API UEmergeBloodRegen : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Water/energy stat level gate: 0=LOW->0.0, 1=MID->0.65, 2=HIGH->1.0; any other -> 1.0 (treat as high).
	UFUNCTION(BlueprintCallable, Category = "Survival|BloodRegen")
	static float RegenStatModifier(int32 StatLevel);

	// Calculates blood regeneration per second based on hydration and energy levels.
	UFUNCTION(BlueprintCallable, Category = "Survival|BloodRegen")
	static float BloodRegenPerSec(int32 WaterLevel, int32 EnergyLevel, bool bUnconscious, float CurrentBlood);

	// Flat blood regen from a saline IV, independent of the gated base rate.
	UFUNCTION(BlueprintCallable, Category = "Survival|BloodRegen")
	static float SalineBoostPerSec();

	// Checks if current blood level is fatal.
	UFUNCTION(BlueprintPure, Category = "Survival|BloodRegen")
	static bool IsFatalBlood(float CurrentBlood);
};