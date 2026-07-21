#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeShock.generated.h"

// Deterministic shock/consciousness math.
// Pure, data-driven, unit-testable headless. No Tick, no rendering, no networking.
UCLASS()
class EMERGECORE_API UEmergeShock : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Calculates shock refill per second based on consciousness and critical blood state.
	UFUNCTION(BlueprintCallable, Category = "Survival|Shock")
	static float ShockRefillPerSec(bool bConscious, bool bUnconsciousCriticalBlood);

	// Calculates shock damage per second based on current blood level.
	UFUNCTION(BlueprintCallable, Category = "Survival|Shock")
	static float ShockDamagePerSec(float CurrentBlood);

	// Calculates shock inflicted per walk update with broken legs.
	UFUNCTION(BlueprintCallable, Category = "Survival|Shock")
	static float BrokenLegsWalkShock(int32 LegTier);

	// Calculates shock inflicted by melee attacks.
	UFUNCTION(BlueprintCallable, Category = "Survival|Shock")
	static float MeleeShock(bool bHeavy);
};