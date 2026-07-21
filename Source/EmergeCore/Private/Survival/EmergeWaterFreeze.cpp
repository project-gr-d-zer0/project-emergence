#include "Survival/EmergeWaterFreeze.h"

bool UEmergeWaterFreeze::IsFrozen(float TempC)
{
	return TempC <= 0.0f;
}

bool UEmergeWaterFreeze::CanDrink(bool bFrozen)
{
	return !bFrozen;
}

float UEmergeWaterFreeze::MeltRatePerTick(float AmbientC, float DeltaT)
{
	return AmbientC > 0.0f ? (AmbientC * 0.1f * DeltaT) : 0.0f;
}