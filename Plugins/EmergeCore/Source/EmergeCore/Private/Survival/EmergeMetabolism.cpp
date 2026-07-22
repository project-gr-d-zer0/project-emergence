#include "Survival/EmergeMetabolism.h"

float UEmergeMetabolism::MetabolicSpeed(int32 Movement)
{
	float s;
	switch (Movement)
	{
	case 1: s = 0.1f; break;
	case 2: s = 0.3f; break;
	case 3: s = 0.6f; break;
	default: s = 0.0f; break;
	}
	return s + 0.01f; // METABOLIC_SPEED_*_BASAL
}

float UEmergeMetabolism::ConsumptionModifier(float Stat)
{
	return Stat / 5000.0f + 1.0f; // /SL_*_MAX + CONSUMPTION_MULTIPLIER_BASE
}

float UEmergeMetabolism::DrainPerTick(int32 Movement, float Stat, float DeltaT)
{
	return MetabolicSpeed(Movement) * ConsumptionModifier(Stat) * DeltaT;
}

bool UEmergeMetabolism::IsLow(float Stat)
{
	return Stat <= 300.0f; // SL_*_LOW threshold
}

float UEmergeMetabolism::LowStatHealthDamage(float Stat, float DeltaT)
{
	if (IsLow(Stat))
	{
		return 0.05f * DeltaT; // LOW_*_DAMAGE_PER_SEC
	}
	return 0.0f;
}