#include "Combat/EmergeSpareMag.h"

int32 UEmergeSpareMag::TotalReserveRounds(int32 SpareMagCount, int32 MagSize)
{
	return SpareMagCount * MagSize;
}

int32 UEmergeSpareMag::TotalCarried(int32 LoadedRounds, int32 SpareMagCount, int32 MagSize)
{
	return LoadedRounds + SpareMagCount * MagSize;
}

bool UEmergeSpareMag::HasSpareMag(int32 SpareMagCount)
{
	return SpareMagCount > 0;
}