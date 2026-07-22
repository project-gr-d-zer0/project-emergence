#include "Survival/EmergeTemperature.h"

float UEmergeTemperature::BodyTempDeltaPerSec(float AmbientC, float Wetness01, bool bNearHeatSource)
{
	return (AmbientC - 20.0f) * 0.001f - FMath::Clamp(Wetness01, 0.0f, 1.0f) * 0.02f
		+ (bNearHeatSource ? 0.05f : 0.0f);
}

bool UEmergeTemperature::IsHypothermic(float BodyTempC)
{
	return BodyTempC < 35.0f;
}

bool UEmergeTemperature::IsHyperthermic(float BodyTempC)
{
	return BodyTempC > 39.0f;
}
