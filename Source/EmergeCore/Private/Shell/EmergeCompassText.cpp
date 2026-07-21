#include "Shell/EmergeCompassText.h"

int32 UEmergeCompassText::DegreesInt(float HeadingDeg)
{
	float d = FMath::Fmod(HeadingDeg, 360.0f);
	if (d < 0.0f) d += 360.0f;
	return FMath::RoundToInt(d) % 360;
}

float UEmergeCompassText::ToMils(float HeadingDeg)
{
	return HeadingDeg * (6400.0f / 360.0f);
}