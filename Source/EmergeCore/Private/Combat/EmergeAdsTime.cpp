#include "Combat/EmergeAdsTime.h"

float UEmergeAdsTime::AdsTimeSec(float BaseTime, float Ergonomics01, float WeightPenalty01)
{
	const float e = FMath::Clamp(Ergonomics01, 0.0f, 1.0f);
	const float w = FMath::Clamp(WeightPenalty01, 0.0f, 1.0f);
	return BaseTime * (2.0f - e) * (1.0f + w);
}