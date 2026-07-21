#include "Shell/EmergeCompassBearing.h"

float UEmergeCompassBearing::RelativeBearingDeg(float TargetBearing, float FacingBearing)
{
	float d = FMath::Fmod(TargetBearing - FacingBearing + 180.0f, 360.0f);
	if (d < 0.0f) d += 360.0f;
	return d - 180.0f;
}

float UEmergeCompassBearing::MarkerOffset(float RelativeBearingDeg, float FovDeg)
{
	return FMath::Clamp(RelativeBearingDeg / (FovDeg / 2.0f), -1.0f, 1.0f);
}

bool UEmergeCompassBearing::IsInView(float RelativeBearingDeg, float FovDeg)
{
	return FMath::Abs(RelativeBearingDeg) <= (FovDeg / 2.0f);
}