#include "Survival/EmergeItemWetness.h"

float UEmergeItemWetness::WeightMultiplier(int32 WetnessState)
{
	switch (WetnessState)
	{
	case 2: return 1.33f;
	case 3: return 1.66f;
	case 4: return 2.0f;
	default: return 1.0f;
	}
}

float UEmergeItemWetness::WettingRatePerTick(bool bInLiquid, float DeltaT)
{
	return (bInLiquid ? 0.015f : 0.0020f) * DeltaT;
}

float UEmergeItemWetness::DryingRatePerTick(bool bOnGround, float DeltaT)
{
	return (bOnGround ? -0.0008f : -0.0016f) * DeltaT;
}