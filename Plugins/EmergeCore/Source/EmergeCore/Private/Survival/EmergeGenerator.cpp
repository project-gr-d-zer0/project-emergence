#include "Survival/EmergeGenerator.h"

float UEmergeGenerator::TotalPowerDraw(int32 NumLarge, int32 NumMedium, int32 NumSmall)
{
	return 0.02f + NumLarge * 0.09f + NumMedium * 0.03f + NumSmall * 0.01f;
}

float UEmergeGenerator::FuelBurnPerHour(float TotalPowerDraw, float ConsumptionMult)
{
	return TotalPowerDraw * ConsumptionMult;
}

float UEmergeGenerator::RuntimeHours(float Fuel, float FuelBurnPerHour)
{
	if (FuelBurnPerHour <= 0.f)
	{
		return TNumericLimits<float>::Max();
	}
	return Fuel / FuelBurnPerHour;
}

float UEmergeGenerator::FuelAfterHours(float Fuel, float FuelBurnPerHour, float Hours)
{
	return FMath::Max(0.f, Fuel - FuelBurnPerHour * Hours);
}
