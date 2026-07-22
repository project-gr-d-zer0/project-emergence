#include "AI/EmergeDirectorIntensity.h"

float UEmergeDirectorIntensity::IntensityFromDamage(float DamageFraction)
{
	return FMath::Clamp(DamageFraction, 0.f, 1.f);
}

float UEmergeDirectorIntensity::IntensityFromNearbyDeath(float DistUu)
{
	return 0.3f * FMath::Clamp(300.f / FMath::Max(DistUu, 300.f), 0.f, 1.f);
}

float UEmergeDirectorIntensity::Decay(float Current, float Dt, bool bEnemiesEngaged)
{
	if (bEnemiesEngaged)
	{
		return Current;
	}
	return Current * FMath::Pow(0.5f, Dt / 10.f);
}
