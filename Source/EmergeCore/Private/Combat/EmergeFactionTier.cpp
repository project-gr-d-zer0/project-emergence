#include "Combat/EmergeFactionTier.h"

float UEmergeFactionTier::ArmorValueForTier(int32 Tier)
{
	return static_cast<float>(FMath::Clamp(Tier, 0, 4)) * 20.0f;
}

int32 UEmergeFactionTier::BackpackSlotsForTier(int32 Tier)
{
	return 12 + FMath::Clamp(Tier, 0, 4) * 6;
}

bool UEmergeFactionTier::UsesAutomaticWeapons(int32 Tier)
{
	return Tier >= 1;
}
