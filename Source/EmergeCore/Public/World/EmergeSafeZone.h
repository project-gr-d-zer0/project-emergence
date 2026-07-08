#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSafeZone.generated.h"

// Safe-zone loot securing rules: die outside a safe zone -> lose everything;
// die inside a safe zone -> keep all gear. Pure, deterministic, unit-testable.
UCLASS()
class EMERGECORE_API UEmergeSafeZone : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Returns whether gear is kept on death: true only if the death occurred inside a safe zone.
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	static bool GearKeptOnDeath(bool bDiedInSafeZone);

	// Loot value that would be lost if death occurred right now: 0 inside a safe zone,
	// otherwise the carried value (clamped to non-negative).
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	static int32 ValueAtRisk(int32 CarriedValue, bool bInSafeZone);
};
