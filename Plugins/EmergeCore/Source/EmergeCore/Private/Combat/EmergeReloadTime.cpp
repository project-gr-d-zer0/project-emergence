#include "Combat/EmergeReloadTime.h"

float UEmergeReloadTime::ReloadTimeSec(float BaseTime, float Ergonomics01, bool bEmptyReload)
{
	const float e = FMath::Clamp(Ergonomics01, 0.0f, 1.0f);
	return BaseTime * (2.0f - e) * (bEmptyReload ? 1.3f : 1.0f);
}