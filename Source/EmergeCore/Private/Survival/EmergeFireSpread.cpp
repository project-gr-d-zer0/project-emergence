#include "Survival/EmergeFireSpread.h"

float UEmergeFireSpread::SpreadChance(int32 Weather, bool bDryFuel)
{
	float c;
	switch(Weather)
	{
		case 0: c = 0.2f; break;
		case 1: c = 0.05f; break;
		case 2: c = 0.02f; break;
		case 3: c = 0.1f; break;
		default: c = 0.f;
	}
	if (bDryFuel) c *= 2.0f;
	return FMath::Clamp(c, 0.0f, 1.0f);
}

bool UEmergeFireSpread::Spreads(float Roll01, float Chance)
{
	return Roll01 < Chance;
}