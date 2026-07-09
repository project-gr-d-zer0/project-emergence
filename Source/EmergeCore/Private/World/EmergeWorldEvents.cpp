#include "World/EmergeWorldEvents.h"

int32 UEmergeWorldEvents::SurgeDepthShift(int32 BaseDepth, bool bSurgeActive)
{
	return bSurgeActive ? FMath::Max(0, BaseDepth) + 2 : FMath::Max(0, BaseDepth);
}

bool UEmergeWorldEvents::IsSheltered(bool bIndoors, bool bUnderground)
{
	return bIndoors || bUnderground;
}

float UEmergeWorldEvents::BroadcastVisibilityWeight(int32 ValueAtRisk)
{
	return FMath::Clamp((ValueAtRisk - 500) / 4500.0f, 0.0f, 1.0f);
}
