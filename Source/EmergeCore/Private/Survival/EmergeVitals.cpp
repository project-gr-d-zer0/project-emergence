#include "Survival/EmergeVitals.h"

float UEmergeVitals::Deplete(float Current, float RatePerSec, float DeltaSeconds)
{
	return FMath::Clamp(Current - RatePerSec * DeltaSeconds, 0.0f, 100.0f);
}

bool UEmergeVitals::IsStarving(float Hydration)
{
	return Hydration <= 0.0f;
}

float UEmergeVitals::StarvationDamage(float Hydration, float DeltaSeconds, float DamagePerSec)
{
	return (Hydration <= 0.0f) ? DamagePerSec * DeltaSeconds : 0.0f;
}
