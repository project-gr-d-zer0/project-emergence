#include "Survival/EmergeWetness.h"

float UEmergeWetness::WetDeltaPerTick(bool bRaining, bool bInBackpack, float WindStrength01)
{
	float d = bRaining ? 0.01f : -0.00005f;
	d *= (1.0f + FMath::Clamp(WindStrength01, 0.0f, 1.0f) * 0.25f);
	if (bInBackpack) d *= 0.1f;
	return d;
}

bool UEmergeWetness::IsSoakingWet(float Wetness)
{
	return Wetness >= 0.5f;
}

bool UEmergeWetness::WetPenaltyActive(float Wetness)
{
	return Wetness >= 0.5f;
}

float UEmergeWetness::SoakedWeightMultiplier(bool bSoaked)
{
	return bSoaked ? 1.66f : 1.0f;
}