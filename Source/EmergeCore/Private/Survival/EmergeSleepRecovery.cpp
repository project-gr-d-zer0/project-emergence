#include "Survival/EmergeSleepRecovery.h"

float UEmergeSleepRecovery::EnergyRestorePerTick(int32 RestState, float DeltaT)
{
	float rate = 0.0f;
	switch (RestState)
	{
	case 2: rate = 3.0f; break;  // Sleeping
	case 1: rate = 1.0f; break;  // Resting
	default: rate = 0.0f; break; // Active
	}
	return rate * DeltaT;
}

float UEmergeSleepRecovery::FatigueRecoverPerTick(int32 RestState, float DeltaT)
{
	float rate = 0.0f;
	switch (RestState)
	{
	case 2: rate = 5.0f; break;  // Sleeping
	case 1: rate = 2.0f; break;  // Resting
	default: rate = 0.0f; break; // Active
	}
	return rate * DeltaT;
}

bool UEmergeSleepRecovery::IsResting(int32 RestState)
{
	return RestState >= 1;
}