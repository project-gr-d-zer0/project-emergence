#include "Survival/EmergeRainCollect.h"

float UEmergeRainCollect::CollectRatePerTick(int32 Weather, float ContainerArea, float DeltaT)
{
	float r;
	switch (Weather)
	{
	case 1: r = 0.5f; break;
	case 2: r = 0.1f; break;
	case 3: r = 1.0f; break;
	default: r = 0.f; break;
	}
	return r * ContainerArea * DeltaT;
}

float UEmergeRainCollect::FilledAfter(float Current, float Rate, float Seconds, float Capacity)
{
	return FMath::Min(Capacity, Current + Rate * Seconds);
}