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

	// Deterministic owner resolution for an item sitting inside N overlapping safe zones.
	// Owner = argmin over candidates of RegistrationTimestamp, tie-broken by smallest ZoneId
	// (ties on timestamp cannot actually occur since timestamps are unique, but the tie-break
	// keeps the function a well-defined total order regardless). Arrays are parallel and index-
	// aligned (CandidateZoneIds[i] was registered at RegistrationTimestamps[i]).
	// Returns 0 (no-owner sentinel) if CandidateZoneIds is empty or the arrays are mismatched.
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	static int32 ResolveZoneOwner(const TArray<int32>& CandidateZoneIds, const TArray<int64>& RegistrationTimestamps);

	// True iff ResolveZoneOwner would return a real owner (non-zero) for the given candidates.
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	static bool IsItemSecuredByZones(const TArray<int32>& CandidateZoneIds, const TArray<int64>& RegistrationTimestamps);
};
