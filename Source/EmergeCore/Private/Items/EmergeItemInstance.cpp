#include "EmergeItemInstance.h"

bool UEmergeItemInstance::IsBroken(float currentDurability) const {
    return currentDurability <= 0.0f;
}