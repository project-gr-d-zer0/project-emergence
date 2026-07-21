#include "Shell/EmergeInventorySort.h"

int32 UEmergeInventorySort::CompareByWeight(float WeightA, float WeightB)
{
	if (WeightA < WeightB) return -1;
	if (WeightA > WeightB) return 1;
	return 0;
}

int32 UEmergeInventorySort::CompareByValue(int32 ValueA, int32 ValueB)
{
	if (ValueA > ValueB) return -1;
	if (ValueA < ValueB) return 1;
	return 0;
}

bool UEmergeInventorySort::ShouldSwap(float KeyA, float KeyB, bool bAscending)
{
	return bAscending ? (KeyA > KeyB) : (KeyA < KeyB);
}