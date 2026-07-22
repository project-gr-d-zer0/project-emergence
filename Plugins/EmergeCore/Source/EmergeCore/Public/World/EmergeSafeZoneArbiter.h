#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeSafeZoneArbiter.generated.h"

// Per-item cache of the zone ids an item currently overlaps. Kept as a USTRUCT so it can live
// inside a UPROPERTY TMap on the arbiter.
USTRUCT()
struct FEmergeSafeZoneItemPlacement
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<int32> OverlappingZoneIds;
};

// Resolves deterministic loot ownership for items sitting inside overlapping safe zones.
// Registration/placement state is stored here; the actual argmin-by-timestamp formula is the
// pure, stateless UEmergeSafeZone::ResolveZoneOwner. Because that formula is a pure function of
// unchanging inputs, ownership never flickers across ticks.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeSafeZoneArbiter : public UObject
{
	GENERATED_BODY()
public:
	// Records (or updates) a zone's unique, monotonically increasing registration timestamp.
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	void RegisterZone(int32 ZoneId, int64 RegistrationTimestamp);

	// Records which zones an item currently overlaps (may be empty for "in no zone").
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	void PlaceItem(int32 ItemId, const TArray<int32>& OverlappingZoneIds);

	// No-op advance. Resolution is recomputed on demand from stable inputs, so nothing needs to
	// be advanced here; the method exists so callers can drive a per-frame tick without the
	// owner/secured state ever changing.
	UFUNCTION(BlueprintCallable, Category = "SafeZone")
	void Tick();

	// Deterministic owner zone id for ItemId (0 = no owner / not placed in any zone).
	UFUNCTION(BlueprintPure, Category = "SafeZone")
	int32 GetOwnerZoneId(int32 ItemId) const;

	// True iff ItemId currently resolves to a real owner zone.
	UFUNCTION(BlueprintPure, Category = "SafeZone")
	bool IsItemSecured(int32 ItemId) const;

private:
	UPROPERTY()
	TMap<int32, int64> ZoneRegistrationTimestamps;

	UPROPERTY()
	TMap<int32, FEmergeSafeZoneItemPlacement> ItemPlacements;
};
