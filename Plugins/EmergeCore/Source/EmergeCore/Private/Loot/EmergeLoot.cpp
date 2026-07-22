#include "Loot/EmergeLoot.h"
#include "Math/RandomStream.h"

int32 UEmergeLoot::RollItemRarity(int32 Seed, int32 DepthTier)
{
	FRandomStream Stream(Seed);
	return Stream.RandRange(0, FMath::Clamp(DepthTier, 0, 4));
}
