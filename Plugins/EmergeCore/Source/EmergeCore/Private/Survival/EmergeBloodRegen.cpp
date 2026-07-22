#include "Survival/EmergeBloodRegen.h"

float UEmergeBloodRegen::RegenStatModifier(int32 StatLevel)
{
	switch (StatLevel)
	{
	case 0:
		return 0.0f;
	case 1:
		return 0.65f;
	case 2:
		return 1.0f;
	default:
		return 1.0f; // Treat any other value as HIGH
	}
}

float UEmergeBloodRegen::BloodRegenPerSec(int32 WaterLevel, int32 EnergyLevel, bool bUnconscious, float CurrentBlood)
{
	float r = 0.3f * RegenStatModifier(WaterLevel) * RegenStatModifier(EnergyLevel);
	if (bUnconscious && CurrentBlood <= 3000.f)
	{
		r *= 2.0f;
	}
	return r;
}

float UEmergeBloodRegen::SalineBoostPerSec()
{
	return 3.0f; // Flat blood regen from a saline IV
}

bool UEmergeBloodRegen::IsFatalBlood(float CurrentBlood)
{
	return CurrentBlood <= 2500.f;
}