#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeReputation.generated.h"

// Faction reputation/standing: reputation (-100..100) changes with actions; standing tiers
// gate access (e.g. safe-zone groups, traders). Pure, deterministic formulas.
UCLASS()
class EMERGECORE_API UEmergeReputation : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Applies Delta to Current, clamped to [-100, 100].
	UFUNCTION(BlueprintCallable, Category = "Economy|Reputation")
	static int32 ApplyRepChange(int32 Current, int32 Delta);

	// Standing tier: Rep < -50 -> 0 (Hostile); -50..49 -> 1 (Neutral); 50..89 -> 2 (Friendly); >=90 -> 3 (Allied).
	UFUNCTION(BlueprintCallable, Category = "Economy|Reputation")
	static int32 StandingTier(int32 Rep);

	// Whether the actor is allowed into a safe zone (hostiles are turned away).
	UFUNCTION(BlueprintCallable, Category = "Economy|Reputation")
	static bool CanAccessSafeZone(int32 Rep);
};
