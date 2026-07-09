#include "Sensor/EmergeInfluence.h"

float UEmergeInfluence::StampFalloff(float Strength, float Dist, float MaxDist, int32 CurvePow)
{
	if (MaxDist <= 0.0f || Dist >= MaxDist)
	{
		return 0.0f;
	}
	const float T = FMath::Clamp(Dist / MaxDist, 0.0f, 1.0f);
	return Strength * (1.0f - FMath::Pow(T, (float)FMath::Max(1, CurvePow)));
}

float UEmergeInfluence::Tension(float Threat, float Friendly)
{
	return Threat + Friendly;
}

float UEmergeInfluence::NetControl(float Friendly, float Threat)
{
	return Friendly - Threat;
}
