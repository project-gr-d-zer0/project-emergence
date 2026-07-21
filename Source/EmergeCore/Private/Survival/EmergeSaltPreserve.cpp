#include "Survival/EmergeSaltPreserve.h"

float UEmergeSaltPreserve::SpoilRateMultiplier(bool bSalted)
{
	return bSalted ? 0.3f : 1.0f;
}

float UEmergeSaltPreserve::PreservedSpoilPerTick(float BaseSpoilRate, bool bSalted, float DeltaT)
{
	return BaseSpoilRate * SpoilRateMultiplier(bSalted) * DeltaT;
}