#include "Shell/EmergeDamageIndicator.h"

float UEmergeDamageIndicator::RelativeBearing(float HitDirDeg, float FacingDeg)
{
	float d = FMath::Fmod(HitDirDeg - FacingDeg, 360.f);
	if (d < 0.f) d += 360.f;
	return d;
}

int32 UEmergeDamageIndicator::Sector8(float RelativeBearing)
{
	return ((int32)FMath::RoundToInt(RelativeBearing / 45.0f)) % 8;
}

float UEmergeDamageIndicator::Intensity01(float Distance, float MaxDistance)
{
	return FMath::Clamp(1.0f - Distance / MaxDistance, 0.0f, 1.0f);
}