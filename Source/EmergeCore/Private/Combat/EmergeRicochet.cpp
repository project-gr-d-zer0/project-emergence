#include "Combat/EmergeRicochet.h"

bool UEmergeRicochet::WouldRicochet(float ImpactAngleDeg, float RicochetThreshold)
{
	return ImpactAngleDeg <= RicochetThreshold;
}

float UEmergeRicochet::RetainedVelocity01(float ImpactAngleDeg, float RicochetThreshold)
{
	if (ImpactAngleDeg > RicochetThreshold)
	{
		return 0.0f;
	}
	return 1.0f - ImpactAngleDeg / RicochetThreshold;
}