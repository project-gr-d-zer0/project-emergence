#include "Combat/EmergeFireRate.h"

float UEmergeFireRate::ShotIntervalSec(float Rpm)
{
	return 60.0f / Rpm;
}

int32 UEmergeFireRate::RoundsInWindow(float Rpm, float WindowSec)
{
	return (int32)((Rpm / 60.0f) * WindowSec);
}

bool UEmergeFireRate::CanFire(float LastShotTime, float Now, float Rpm)
{
	return (Now - LastShotTime) >= ShotIntervalSec(Rpm);
}