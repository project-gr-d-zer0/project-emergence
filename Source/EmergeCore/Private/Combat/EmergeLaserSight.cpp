#include "Combat/EmergeLaserSight.h"

float UEmergeLaserSight::HipfireSpreadMultiplier(bool bLaserOn)
{
	return bLaserOn ? 0.6f : 1.0f;
}

bool UEmergeLaserSight::IsVisibleToEnemy(float Distance, float MaxVisRange, bool bLaserOn)
{
	return bLaserOn && (Distance <= MaxVisRange);
}