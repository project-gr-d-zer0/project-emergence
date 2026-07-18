#include "Combat/EmergeCombatBallistics.h"

float UEmergeCombatBallistics::ComputeRangeFalloff(float InDistance, float MinRange, float MaxRange)
{
	// Degenerate band: guard first, before any division, so we never compute NaN/Inf.
	if (MaxRange <= MinRange)
	{
		return 1.0f;
	}

	const float Factor = (MaxRange - InDistance) / (MaxRange - MinRange);
	return FMath::Clamp(Factor, 0.0f, 1.0f);
}
