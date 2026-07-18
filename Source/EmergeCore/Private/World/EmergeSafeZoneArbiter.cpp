#include "World/EmergeSafeZoneArbiter.h"
#include "World/EmergeSafeZone.h"

void UEmergeSafeZoneArbiter::RegisterZone(int32 ZoneId, int64 RegistrationTimestamp)
{
	ZoneRegistrationTimestamps.Add(ZoneId, RegistrationTimestamp);
}

void UEmergeSafeZoneArbiter::PlaceItem(int32 ItemId, const TArray<int32>& OverlappingZoneIds)
{
	FEmergeSafeZoneItemPlacement Placement;
	Placement.OverlappingZoneIds = OverlappingZoneIds;
	ItemPlacements.Add(ItemId, Placement);
}

void UEmergeSafeZoneArbiter::Tick()
{
	// Intentionally empty: ownership is a pure function of registration/placement state, which
	// does not change on tick, so there is nothing to advance.
}

int32 UEmergeSafeZoneArbiter::GetOwnerZoneId(int32 ItemId) const
{
	const FEmergeSafeZoneItemPlacement* Placement = ItemPlacements.Find(ItemId);
	if (!Placement || Placement->OverlappingZoneIds.Num() == 0)
	{
		return 0;
	}

	TArray<int32> CandidateZoneIds;
	TArray<int64> CandidateTimestamps;
	CandidateZoneIds.Reserve(Placement->OverlappingZoneIds.Num());
	CandidateTimestamps.Reserve(Placement->OverlappingZoneIds.Num());
	for (int32 ZoneId : Placement->OverlappingZoneIds)
	{
		if (const int64* Timestamp = ZoneRegistrationTimestamps.Find(ZoneId))
		{
			CandidateZoneIds.Add(ZoneId);
			CandidateTimestamps.Add(*Timestamp);
		}
	}

	return UEmergeSafeZone::ResolveZoneOwner(CandidateZoneIds, CandidateTimestamps);
}

bool UEmergeSafeZoneArbiter::IsItemSecured(int32 ItemId) const
{
	return GetOwnerZoneId(ItemId) != 0;
}
