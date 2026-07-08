#include "EmergeItemInstance.h"

// Implementation of IsBroken Method
bool UEmergeItemInstance::IsBroken(float currentDurability) const {
    return currentDurability <= 0.0f;
}