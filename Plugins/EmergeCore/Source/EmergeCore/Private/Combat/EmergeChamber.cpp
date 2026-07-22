#include "Combat/EmergeChamber.h"

int32 UEmergeChamber::TotalWithChamber(int32 MagRounds, bool bChambered)
{
	return MagRounds + (bChambered ? 1 : 0);
}

int32 UEmergeChamber::MaxCapacity(int32 MagSize)
{
	return MagSize + 1;
}

bool UEmergeChamber::CanChamber(int32 MagRounds, bool bAlreadyChambered)
{
	return MagRounds > 0 && !bAlreadyChambered;
}