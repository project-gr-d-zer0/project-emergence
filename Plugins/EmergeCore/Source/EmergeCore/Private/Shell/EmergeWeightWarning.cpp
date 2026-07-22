#include "Shell/EmergeWeightWarning.h"

int32 UEmergeWeightWarning::WarningLevel(float CurrentKg, float MaxKg)
{
	const float f = CurrentKg / MaxKg;
	if (f < 0.8f) return 0;
	if (f < 1.0f) return 1;
	return 2;
}

bool UEmergeWeightWarning::IsOverloaded(float CurrentKg, float MaxKg)
{
	return CurrentKg > MaxKg;
}