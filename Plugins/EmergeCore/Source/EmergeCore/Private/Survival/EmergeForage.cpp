#include "Survival/EmergeForage.h"

int32 UEmergeForage::YieldForArea(int32 Richness, bool bTool)
{
	int32 y;
	switch (Richness)
	{
	case 1: y = 1; break;
	case 2: y = 3; break;
	default: y = 0; break;
	}
	if (bTool && y > 0) y += 1;
	return y;
}

float UEmergeForage::RegrowTimeSec(int32 Richness)
{
	switch (Richness)
	{
	case 1: return 600.0f;
	case 2: return 300.0f;
	default: return -1.0f; // barren never regrows
	}
}

bool UEmergeForage::IsForageable(int32 Richness)
{
	return Richness >= 1;
}