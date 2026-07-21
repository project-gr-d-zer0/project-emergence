#include "Combat/EmergeSpreadStance.h"

float UEmergeSpreadStance::StanceMultiplier(int32 Stance)
{
	switch (Stance)
	{
	case 1: return 0.7f;
	case 2: return 0.5f;
	default: return 1.0f;
	}
}

float UEmergeSpreadStance::EffectiveSpreadMoa(float BaseMoa, int32 Stance, bool bMoving, bool bAds)
{
	return BaseMoa * StanceMultiplier(Stance) * (bMoving ? 1.5f : 1.0f) * (bAds ? 0.5f : 1.0f);
}