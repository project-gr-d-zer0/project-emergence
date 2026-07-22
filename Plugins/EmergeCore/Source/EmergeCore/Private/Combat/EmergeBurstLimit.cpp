#include "Combat/EmergeBurstLimit.h"

int32 UEmergeBurstLimit::RoundsThisBurst(int32 TriggerHeldRounds, int32 BurstSize)
{
	return FMath::Min(TriggerHeldRounds, BurstSize);
}

bool UEmergeBurstLimit::BurstComplete(int32 Fired, int32 BurstSize)
{
	return Fired >= BurstSize;
}

int32 UEmergeBurstLimit::RemainingInBurst(int32 Fired, int32 BurstSize)
{
	return FMath::Max(0, BurstSize - Fired);
}