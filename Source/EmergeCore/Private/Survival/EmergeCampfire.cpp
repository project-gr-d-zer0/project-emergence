#include "Survival/EmergeCampfire.h"

float UEmergeCampfire::FuelBurnedPerTick(bool bLit, float DeltaT)
{
	return bLit ? 0.5f * DeltaT : 0.0f;
}

float UEmergeCampfire::BurnTimeRemaining(float Fuel, float BurnRate)
{
	return BurnRate > 0.0f ? Fuel / BurnRate : 0.0f;
}

bool UEmergeCampfire::IsLit(float Fuel)
{
	return Fuel > 0.0f;
}