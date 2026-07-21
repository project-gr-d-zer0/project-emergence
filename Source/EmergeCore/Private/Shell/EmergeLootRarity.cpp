#include "Shell/EmergeLootRarity.h"

int32 UEmergeLootRarity::RarityTier(int32 Value)
{
	if (Value >= 10000) return 4;
	if (Value >= 2000) return 3;
	if (Value >= 500) return 2;
	if (Value >= 100) return 1;
	return 0;
}

float UEmergeLootRarity::BorderGlow(int32 Tier)
{
	return FMath::Clamp(Tier / 4.0f, 0.0f, 1.0f);
}