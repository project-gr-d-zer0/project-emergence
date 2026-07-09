#include "Sensor/EmergeNavClassify.h"

bool UEmergeNavClassify::IsReachable(int32 QueryResult, bool bIsPartial)
{
	return QueryResult == 3 && !bIsPartial;
}

float UEmergeNavClassify::DetourRatio(float PathLength, float StraightDist)
{
	return (StraightDist <= 0.0f) ? 1.0f : PathLength / StraightDist;
}
