#include "Survival/EmergeWaterPurity.h"

float UEmergeWaterPurity::ContaminationChance(int32 Source)
{
	switch (Source)
	{
	case 0: return 0.0f;
	case 1: return 0.05f;
	case 2: return 0.3f;
	case 3: return 0.6f;
	default: return 0.0f;
	}
}

bool UEmergeWaterPurity::IsSafe(int32 Source)
{
	return ContaminationChance(Source) <= 0.0f;
}

float UEmergeWaterPurity::PurifiedChance(float BaseChance, bool bPurified)
{
	return bPurified ? 0.0f : BaseChance;
}