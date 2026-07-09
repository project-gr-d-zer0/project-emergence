#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStatusTier.generated.h"

// Generic status/vital tier bands: buckets a continuous vital (0..100) into a discrete
// tier used to gate penalties. Deterministic, pure formulas.
UCLASS()
class EMERGECORE_API UEmergeStatusTier : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Value >= 75 -> 0 (fine); 50..74 -> 1 (low); 25..49 -> 2 (bad); < 25 -> 3 (critical).
	UFUNCTION(BlueprintCallable, Category = "Survival|StatusTier")
	static int32 TierForValue(float Value);

	// 0 -> 1.0; 1 -> 0.9; 2 -> 0.75; 3+ -> 0.5.
	UFUNCTION(BlueprintCallable, Category = "Survival|StatusTier")
	static float PenaltyMultiplierForTier(int32 Tier);
};
