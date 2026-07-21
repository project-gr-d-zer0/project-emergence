#include "Combat/EmergeAimMode.h"

float UEmergeAimMode::AccuracyMultiplier(bool bAds)
{
	return bAds ? 0.4f : 1.0f;
}

float UEmergeAimMode::MoveSpeedMultiplier(bool bAds)
{
	return bAds ? 0.7f : 1.0f;
}