#include "Sensor/EmergeNavFollow.h"

bool UEmergeNavFollow::ArrivedAt(float DistToGoal, float AcceptRadius)
{
	return DistToGoal <= AcceptRadius;
}

bool UEmergeNavFollow::ShouldAdvanceWaypoint(float DistToWaypoint, float AdvanceRadius)
{
	return DistToWaypoint <= AdvanceRadius;
}

float UEmergeNavFollow::NavProgress(float TotalPathLen, float RemainingLen)
{
	return (TotalPathLen <= 0.0f) ? 1.0f : FMath::Clamp(1.0f - RemainingLen / TotalPathLen, 0.0f, 1.0f);
}
