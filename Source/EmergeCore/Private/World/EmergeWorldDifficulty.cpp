#include "World/EmergeWorldDifficulty.h"

int32 UEmergeWorldDifficulty::HordeDensityAtDepth(int32 Depth)
{
	const int32 D = FMath::Max(0, Depth);
	return 10 + D * 15;
}

int32 UEmergeWorldDifficulty::EnemyGearTierAtDepth(int32 Depth)
{
	return FMath::Clamp(Depth, 0, 4);
}

float UEmergeWorldDifficulty::HordeSpawnIntervalSeconds(int32 Depth)
{
	return FMath::Max(2.0f, 20.0f - FMath::Max(0, Depth) * 2.0f);
}
