#include "Combat/EmergeHeadshotHelmet.h"

float UEmergeHeadshotHelmet::HeadshotMultiplier(float BaseHeadMult, int32 HelmetClass)
{
	const float r = FMath::Clamp(HelmetClass * 0.15f, 0.0f, 0.9f);
	return BaseHeadMult * (1.0f - r);
}

bool UEmergeHeadshotHelmet::StopsHeadshotKill(int32 HelmetClass, float PenPower)
{
	return PenPower < (HelmetClass * 10.0f);
}