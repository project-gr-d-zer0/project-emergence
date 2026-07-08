#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBallistics.generated.h"

// Core ballistics formulas: penetration vs armor class, and range-based damage falloff.
// Pure/deterministic math library (spec: core ballistics differentiator).
UCLASS()
class EMERGECORE_API UEmergeBallistics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Armor stops the round when PenetrationValue < ArmorClass * 10.
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static bool Penetrates(float PenetrationValue, int32 ArmorClass);

	// Linear damage falloff: full damage up to FalloffStartM, decaying to 50% at/after FalloffEndM.
	// If FalloffEndM <= FalloffStartM, returns BaseDamage (degenerate range guard).
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static float ComputeDamage(float BaseDamage, float DistanceM, float FalloffStartM, float FalloffEndM);

	// Applies armor mitigation to an already-computed damage value.
	// If the round failed to penetrate, only a 15% bleed-through fraction gets through.
	// If it penetrated, damage is still reduced by residual armor, up to a 60% cap.
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static float MitigatedDamage(float RawDamage, float ArmorValue, bool bPenetrated);
};
