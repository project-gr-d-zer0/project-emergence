#include "Combat/EmergeFlashlightBlind.h"

float UEmergeFlashlightBlind::BlindIntensity(float Distance, float MaxRange, bool bFacing)
{
    if (!bFacing) return 0.0f;
    if (Distance >= MaxRange) return 0.0f;
    return 1.0f - Distance / MaxRange;
}

bool UEmergeFlashlightBlind::IsBlinded(float Intensity, float Threshold)
{
    return Intensity >= Threshold;
}