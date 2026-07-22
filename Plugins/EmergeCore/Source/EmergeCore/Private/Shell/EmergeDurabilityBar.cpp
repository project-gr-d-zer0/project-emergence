#include "Shell/EmergeDurabilityBar.h"

float UEmergeDurabilityBar::Fill01(float Current, float Max)
{
	return FMath::Clamp(Current / Max, 0.0f, 1.0f);
}

int32 UEmergeDurabilityBar::ColorTier(float Fill01)
{
	if (Fill01 > 0.5f) return 0;
	if (Fill01 > 0.2f) return 1;
	return 2;
}

bool UEmergeDurabilityBar::IsBroken(float Current)
{
	return Current <= 0.0f;
}