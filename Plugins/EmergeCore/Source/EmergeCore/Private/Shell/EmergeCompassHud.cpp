#include "Shell/EmergeCompassHud.h"

float UEmergeCompassHud::NormalizeHeading(float Degrees)
{
	float d = FMath::Fmod(Degrees, 360.0f);
	if (d < 0.0f) d += 360.0f;
	return d;
}

int32 UEmergeCompassHud::Cardinal8(float Degrees)
{
	return ((int32)FMath::RoundToInt(NormalizeHeading(Degrees) / 45.0f)) % 8;
}