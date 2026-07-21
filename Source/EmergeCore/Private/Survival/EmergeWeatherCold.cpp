#include "Survival/EmergeWeatherCold.h"

float UEmergeWeatherCold::ColdRatePerTick(int32 Weather, bool bSheltered, float DeltaT)
{
	float r;
	switch (Weather)
	{
	case 1: r = 0.02f; break;
	case 2: r = 0.05f; break;
	case 3: r = 0.1f; break;
	default: r = 0.f; break;
	}
	if (bSheltered) r *= 0.5f;
	return r * DeltaT;
}

bool UEmergeWeatherCold::IsFreezingWeather(int32 Weather)
{
	return Weather >= 2;
}