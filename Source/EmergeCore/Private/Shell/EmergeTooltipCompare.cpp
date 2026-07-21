#include "Shell/EmergeTooltipCompare.h"

int32 UEmergeTooltipCompare::CompareStat(float NewVal, float CurrentVal, bool bHigherBetter)
{
	if (NewVal == CurrentVal) return 0;
	const bool bBetter = bHigherBetter ? (NewVal > CurrentVal) : (NewVal < CurrentVal);
	return bBetter ? 1 : -1;
}

float UEmergeTooltipCompare::StatDelta(float NewVal, float CurrentVal)
{
	return NewVal - CurrentVal;
}