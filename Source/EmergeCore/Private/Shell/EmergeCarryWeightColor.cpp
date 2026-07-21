#include "Shell/EmergeCarryWeightColor.h"

float UEmergeCarryWeightColor::WeightFraction01(float Current, float Max)
{
	return FMath::Clamp(Current / Max, 0.0f, 1.0f);
}

int32 UEmergeCarryWeightColor::WeightColorTier(float Fraction01)
{
	if (Fraction01 < 0.5f) return 0;
	if (Fraction01 < 0.85f) return 1;
	if (Fraction01 < 1.0f) return 2;
	return 3;
}