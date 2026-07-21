#include "Combat/EmergeMagCheck.h"

int32 UEmergeMagCheck::AmmoBand(int32 Current, int32 MagSize)
{
	if (Current <= 0) return 0;
	if (Current >= MagSize) return 4;

	const float f = (float)Current / (float)MagSize;
	if (f < 0.25f) return 1;
	if (f < 0.75f) return 2;
	return 3;
}

bool UEmergeMagCheck::NeedsReload(int32 Current, int32 MagSize)
{
	return AmmoBand(Current, MagSize) <= 1;
}