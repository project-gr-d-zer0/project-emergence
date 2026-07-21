#include "Shell/EmergeHotbar.h"

int32 UEmergeHotbar::WrapSlot(int32 Index, int32 SlotCount)
{
    if (SlotCount <= 0) return 0; // Guard against invalid slot counts
    return ((Index % SlotCount) + SlotCount) % SlotCount;
}

int32 UEmergeHotbar::NextSlot(int32 Current, int32 SlotCount)
{
    return WrapSlot(Current + 1, SlotCount);
}

int32 UEmergeHotbar::PrevSlot(int32 Current, int32 SlotCount)
{
    return WrapSlot(Current - 1, SlotCount);
}