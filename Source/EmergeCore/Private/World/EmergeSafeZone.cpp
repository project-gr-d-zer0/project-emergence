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
