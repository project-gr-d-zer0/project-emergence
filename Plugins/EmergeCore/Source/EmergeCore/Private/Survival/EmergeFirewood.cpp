#include "Survival/EmergeFirewood.h"

int32 UEmergeFirewood::YieldForTool(int32 ToolTier)
{
	switch (ToolTier)
	{
	case 1: return 2;
	case 2: return 4;
	case 3: return 6;
	default: return 1;
	}
}

float UEmergeFirewood::GatherTimeSec(int32 ToolTier)
{
	switch (ToolTier)
	{
	case 1: return 20.0f;
	case 2: return 10.0f;
	case 3: return 6.0f;
	default: return 30.0f;
	}
}

float UEmergeFirewood::BurnValuePerLog()
{
	return 60.0f;
}