#include "World/EmergeSafeZone.h"

bool UEmergeSafeZone::GearKeptOnDeath(bool bDiedInSafeZone)
{
	return bDiedInSafeZone;
}

int32 UEmergeSafeZone::ValueAtRisk(int32 CarriedValue, bool bInSafeZone)
{
	if (bInSafeZone)
	{
		return 0;
	}
	return FMath::Max(0, CarriedValue);
}

int32 UEmergeSafeZone::ResolveZoneOwner(const TArray<int32>& CandidateZoneIds, const TArray<int64>& RegistrationTimestamps)
{
	if (CandidateZoneIds.Num() == 0 || CandidateZoneIds.Num() != RegistrationTimestamps.Num())
	{
		return 0;
	}

	int32 BestIdx = 0;
	for (int32 Index = 1; Index < CandidateZoneIds.Num(); ++Index)
	{
		const bool bEarlier = RegistrationTimestamps[Index] < RegistrationTimestamps[BestIdx];
		const bool bTieBreak = (RegistrationTimestamps[Index] == RegistrationTimestamps[BestIdx]) &&
			(CandidateZoneIds[Index] < CandidateZoneIds[BestIdx]);
		if (bEarlier || bTieBreak)
		{
			BestIdx = Index;
		}
	}
	return CandidateZoneIds[BestIdx];
}

bool UEmergeSafeZone::IsItemSecuredByZones(const TArray<int32>& CandidateZoneIds, const TArray<int64>& RegistrationTimestamps)
{
	return ResolveZoneOwner(CandidateZoneIds, RegistrationTimestamps) != 0;
}
