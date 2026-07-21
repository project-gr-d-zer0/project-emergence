#include "Shell/EmergeRadialMenu.h"

int32 UEmergeRadialMenu::SlotFromAngle(float AngleDeg, int32 SlotCount)
{
    if (SlotCount <= 0) return 0;
    // Normalize angle to [0, 360)
    float NormalizedAngle = FMath::Fmod(AngleDeg, 360.0f);
    if (NormalizedAngle < 0.0f) NormalizedAngle += 360.0f;

    // Calculate the slot by dividing the circle into equal parts
    int32 Slot = FMath::RoundToInt(NormalizedAngle / (360.0f / SlotCount));
    // Wrap around if needed
    return Slot % SlotCount;
}

float UEmergeRadialMenu::SlotCenterAngle(int32 Slot, int32 SlotCount)
{
    if (SlotCount <= 0) return 0.0f;
    // Calculate the center angle for the given slot
    return Slot * (360.0f / SlotCount);
}