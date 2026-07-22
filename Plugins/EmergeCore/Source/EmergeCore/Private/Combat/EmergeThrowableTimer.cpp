#include "Combat/EmergeThrowableTimer.h"

float UEmergeThrowableTimer::FuseRemaining(float FuseTime, float Cooked)
{
	return FMath::Max(0.0f, FuseTime - Cooked);
}

bool UEmergeThrowableTimer::HasDetonated(float FuseTime, float Cooked)
{
	return Cooked >= FuseTime;
}

bool UEmergeThrowableTimer::DetonatesInAir(float FuseTime, float Cooked, float FlightTime)
{
	return FuseRemaining(FuseTime, Cooked) <= FlightTime;
}