#include "Nav/EmergeNavRecovery.h"

int32 UEmergeNavRecovery::DecideRecovery(float StuckSeconds, int32 RepathCount, float StuckThreshold, int32 MaxRepaths)
{
	if (StuckSeconds < StuckThreshold)
	{
		return 0;
	}
	if (RepathCount < MaxRepaths)
	{
		return 1;
	}
	return 3;
}

bool UEmergeNavRecovery::ShouldGiveUp(int32 ActionCode)
{
	return ActionCode == 3;
}
