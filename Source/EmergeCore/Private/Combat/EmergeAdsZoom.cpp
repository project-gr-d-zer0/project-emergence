#include "Combat/EmergeAdsZoom.h"

float UEmergeAdsZoom::ZoomedFov(float BaseFov, float Magnification)
{
	return BaseFov / Magnification;
}

float UEmergeAdsZoom::SensitivityMultiplier(float Magnification)
{
	return 1.0f / Magnification;
}