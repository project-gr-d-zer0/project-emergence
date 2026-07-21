#include "Survival/EmergeHeatSource.h"

float UEmergeHeatSource::HeatAtDistance(float SourceHeat, float Distance, float MaxRange)
{
	if (Distance >= MaxRange) return 0.0f;
	return SourceHeat * (1.0f - Distance / MaxRange);
}

bool UEmergeHeatSource::InWarmthRange(float Distance, float MaxRange)
{
	return Distance < MaxRange;
}

float UEmergeHeatSource::FireplaceHeatingRate(float BaseRate)
{
	return BaseRate * 2.0f; // TEMP_COEF_FIREPLACE_HEATING
}