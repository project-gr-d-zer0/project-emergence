#include "Combat/EmergeHitZone.h"

float UEmergeHitZone::ZoneMultiplier(int32 Zone)
{
	switch (Zone)
	{
	case 0: return 2.0f; // Head
	case 1: return 1.0f; // Thorax
	case 2: return 1.5f; // Stomach
	case 3: return 0.7f; // Arms
	case 4: return 0.7f; // Legs
	default: return 1.0f; // Unknown
	}
}

float UEmergeHitZone::ApplyZone(float BaseDamage, int32 Zone)
{
	return BaseDamage * ZoneMultiplier(Zone);
}