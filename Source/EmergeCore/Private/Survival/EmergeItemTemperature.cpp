#include "Survival/EmergeItemTemperature.h"

int32 UEmergeItemTemperature::TemperatureLevel(float TempC)
{
	if (TempC >= 600.f) return 4;
	if (TempC >= 250.f) return 3;
	if (TempC >= 70.f) return 2;
	if (TempC >= 35.f) return 1;
	if (TempC <= -100.f) return -4;
	if (TempC <= -50.f) return -3;
	if (TempC <= -10.f) return -2;
	if (TempC <= 3.f) return -1;
	return 0;
}

bool UEmergeItemTemperature::IsHot(float TempC)
{
	return TempC >= 35.0f;
}

bool UEmergeItemTemperature::IsFreezing(float TempC)
{
	return TempC <= 3.0f;
}