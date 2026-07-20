#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeGenerator.generated.h"

// Deterministic generator fuel/power math (PZ B41 IsoGenerator.java exact constants).
UCLASS()
class EMERGECORE_API UEmergeGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Base draw plus per-appliance load (large/medium/small).
	UFUNCTION(BlueprintCallable, Category = "Survival|Generator")
	static float TotalPowerDraw(int32 NumLarge, int32 NumMedium, int32 NumSmall);

	// Fuel burned per hour = TotalPowerDraw * ConsumptionMult.
	UFUNCTION(BlueprintCallable, Category = "Survival|Generator")
	static float FuelBurnPerHour(float TotalPowerDraw, float ConsumptionMult);

	// Hours until the tank empties. Guarded against divide-by-zero.
	UFUNCTION(BlueprintCallable, Category = "Survival|Generator")
	static float RuntimeHours(float Fuel, float FuelBurnPerHour);

	// Fuel remaining after Hours of runtime, clamped at 0.
	UFUNCTION(BlueprintCallable, Category = "Survival|Generator")
	static float FuelAfterHours(float Fuel, float FuelBurnPerHour, float Hours);
};
