#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBallisticAccuracyResolver.generated.h"

// Deterministic, data-free hit-chance resolver: combines a non-linear encumbrance decay with
// multiplicative vital-status penalties. Pure math -> BlueprintFunctionLibrary per constitution.
UCLASS()
class EMERGECORE_API UEmergeBallisticAccuracyResolver : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// EncumbranceTier: 0=Unburdened,1=Light,2=Heavy,3=Overloaded.
	// EncumbranceMultiplier = 1 - 0.09 * (tier^2).
	// VitalMultiplier = (bBleeding ? 0.85 : 1.0) * (bStunned ? 0.50 : 1.0).
	// Result = clamp(BaseAccuracy * EncumbranceMultiplier * VitalMultiplier, 0, 100), rounded to 2 decimals.
	UFUNCTION(BlueprintCallable, Category = "Combat|Ballistics")
	static float ResolveHitChance(float BaseAccuracy, int32 EncumbranceTier, bool bBleeding, bool bStunned);
};
