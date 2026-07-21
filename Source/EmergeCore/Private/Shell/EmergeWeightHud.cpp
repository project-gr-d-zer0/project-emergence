#include "Shell/EmergeWeightHud.h"
#include "Math/UnrealMathUtility.h"

float UEmergeWeightHud::PenalizedWeightKg(float WeightGrams)
{
	return FMath::Max(0.0f, (WeightGrams - 6000.0f) / 1000.0f);
}

float UEmergeWeightHud::StaminaPenalty(float WeightGrams)
{
	return PenalizedWeightKg(WeightGrams) * 1.75f;
}

float UEmergeWeightHud::MaxStaminaForWeight(float WeightGrams)
{
	return FMath::Clamp(100.0f - StaminaPenalty(WeightGrams), 5.0f, 100.0f);
}

bool UEmergeWeightHud::IsOverloaded(float WeightGrams)
{
	return WeightGrams > 6000.0f;
}