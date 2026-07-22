#include "Survival/EmergeShock.h"

float UEmergeShock::ShockRefillPerSec(bool bConscious, bool bUnconsciousCriticalBlood)
{
	if (bConscious) return 5.0f;
	return bUnconsciousCriticalBlood ? 1.0f * 0.8f : 1.0f;
}

float UEmergeShock::ShockDamagePerSec(float CurrentBlood)
{
	if (CurrentBlood > 3000.f) return 0.0f;
	if (CurrentBlood <= 2500.f) return 6.25f;
	const float T = (3000.f - CurrentBlood) / 500.f; // 0 at 3000, 1 at 2500
	return 5.5f + T * (6.25f - 5.5f);
}

float UEmergeShock::BrokenLegsWalkShock(int32 LegTier)
{
	switch (LegTier)
	{
	case 0: return 4.0f;
	case 1: return 6.0f;
	case 2: return 8.0f;
	default: return 4.0f;
	}
}

float UEmergeShock::MeleeShock(bool bHeavy)
{
	return bHeavy ? 25.0f : 10.0f;
}