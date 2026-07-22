#include "Survival/EmergeStatusTier.h"

int32 UEmergeStatusTier::TierForValue(float Value)
{
	if (Value >= 75.0f)
	{
		return 0;
	}
	if (Value >= 50.0f)
	{
		return 1;
	}
	if (Value >= 25.0f)
	{
		return 2;
	}
	return 3;
}

float UEmergeStatusTier::PenaltyMultiplierForTier(int32 Tier)
{
	if (Tier <= 0)
	{
		return 1.0f;
	}
	if (Tier == 1)
	{
		return 0.9f;
	}
	if (Tier == 2)
	{
		return 0.75f;
	}
	return 0.5f;
}
