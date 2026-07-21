#include "Combat/EmergeAimSway.h"

float UEmergeAimSway::SwayAmplitude(float BaseSway, bool bHoldingBreath, float Stamina01)
{
	const float s = FMath::Clamp(Stamina01, 0.0f, 1.0f);
	return BaseSway * (bHoldingBreath ? 0.3f : 1.0f) * (1.0f + (1.0f - s) * 0.5f);
}

bool UEmergeAimSway::CanHoldBreath(float Stamina01)
{
	return Stamina01 > 0.1f;
}