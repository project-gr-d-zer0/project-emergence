#include "Shell/EmergeHitmarker.h"

int32 UEmergeHitmarker::MarkerState(bool bHit, bool bArmor, bool bHeadshot, bool bKill)
{
	if (!bHit) return 0;
	if (bKill) return 4;
	if (bHeadshot) return 3;
	if (bArmor) return 2;
	return 1;
}

float UEmergeHitmarker::MarkerScale(int32 State)
{
	switch (State)
	{
	case 4: return 1.5f; // Kill
	case 3: return 1.25f; // Headshot
	default: return 1.0f; // Hit, Armor, None
	}
}