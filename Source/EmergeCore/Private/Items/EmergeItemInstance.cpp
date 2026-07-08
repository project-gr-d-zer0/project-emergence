#include "Items/EmergeItemInstance.h"

bool UEmergeItemInstance::IsBroken(float CurrentDurability)
{
	return CurrentDurability <= 0.0f;
}
