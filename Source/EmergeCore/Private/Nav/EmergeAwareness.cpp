#include "Nav/EmergeAwareness.h"

float UEmergeAwareness::FillRate(float Dist, float MaxDist, bool bHasLineOfSight)
{
	if (!bHasLineOfSight || MaxDist <= 0.0f || Dist >= MaxDist)
	{
		return 0.0f;
	}
	return 1.0f - FMath::Clamp(Dist / MaxDist, 0.0f, 1.0f);
}

float UEmergeAwareness::Accumulate(float Current, float FillRate, float DtSeconds)
{
	return FMath::Clamp(Current + FillRate * FMath::Max(0.0f, DtSeconds), 0.0f, 1.0f);
}

float UEmergeAwareness::Decay(float Current, float DecayPerSecond, float DtSeconds)
{
	return FMath::Clamp(Current - FMath::Max(0.0f, DecayPerSecond) * FMath::Max(0.0f, DtSeconds), 0.0f, 1.0f);
}

int32 UEmergeAwareness::StateFor(float Awareness)
{
	if (Awareness < 0.25f)
	{
		return 0;
	}
	if (Awareness < 0.60f)
	{
		return 1;
	}
	if (Awareness < 1.0f)
	{
		return 2;
	}
	return 3;
}
