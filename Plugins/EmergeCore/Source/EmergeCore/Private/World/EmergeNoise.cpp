#include "World/EmergeNoise.h"

float UEmergeNoise::EffectiveRadius(float BaseRadius, float EnvMult, float DepthMult)
{
	return FMath::Max(0.0f, BaseRadius * EnvMult * DepthMult);
}

bool UEmergeNoise::IsHeard(float EffectiveRadiusM, float DistanceM)
{
	return DistanceM >= 0.0f && DistanceM <= EffectiveRadiusM;
}

float UEmergeNoise::AttractionWeight(float Volume, float DistanceM)
{
	return FMath::Max(0.0f, Volume) / FMath::Max(1.0f, DistanceM);
}

EEmergeNoiseResponse UEmergeNoise::ResponseToShot(bool bSuppressed, bool bHeard, bool bLineOfSight)
{
	if (!bHeard)
	{
		return EEmergeNoiseResponse::Ignore;
	}
	if (!bSuppressed)
	{
		return EEmergeNoiseResponse::TargetExact;
	}
	return bLineOfSight ? EEmergeNoiseResponse::TargetExact : EEmergeNoiseResponse::Investigate;
}

float UEmergeNoise::FootstepRadiusM(float SpeedRatio)
{
	return 2.0f + 18.0f * FMath::Clamp(SpeedRatio, 0.0f, 1.0f);
}

float UEmergeNoise::GunshotRadiusM(bool bSuppressed)
{
	return bSuppressed ? 120.0f : 800.0f;
}
