#include "Loot/EmergeLootRespawn.h"

int32 UEmergeLootRespawn::SpawnDeficit(int32 Nominal, int32 Current, int32 MinCount)
{
	if (Current > MinCount)
	{
		return 0;
	}
	return FMath::Max(0, Nominal - Current);
}

bool UEmergeLootRespawn::ShouldDespawn(float AgeSeconds, float LifetimeSeconds)
{
	return LifetimeSeconds > 0.0f && AgeSeconds >= LifetimeSeconds;
}

bool UEmergeLootRespawn::RestockReady(float SinceLastRestockSeconds, float RestockDelaySeconds)
{
	return SinceLastRestockSeconds >= FMath::Max(0.0f, RestockDelaySeconds);
}
