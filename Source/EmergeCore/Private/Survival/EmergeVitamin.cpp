#include "Survival/EmergeVitamin.h"

float UEmergeVitamin::LifetimeSecs()
{
	return 300.0f;
}

float UEmergeVitamin::RemainingSecs(float Elapsed)
{
	return FMath::Max(0.0f, LifetimeSecs() - Elapsed);
}

bool UEmergeVitamin::IsActive(float Elapsed)
{
	return Elapsed < LifetimeSecs();
}

int32 UEmergeVitamin::BoostedImmunityLevel()
{
	return 0;
}