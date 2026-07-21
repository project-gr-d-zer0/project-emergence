#include "Shell/EmergeObjectiveTracker.h"

float UEmergeObjectiveTracker::Progress01(int32 Current, int32 Target)
{
	return FMath::Clamp((float)Current / (float)Target, 0.f, 1.f);
}

bool UEmergeObjectiveTracker::IsComplete(int32 Current, int32 Target)
{
	return Current >= Target;
}

int32 UEmergeObjectiveTracker::Remaining(int32 Current, int32 Target)
{
	return FMath::Max(0, Target - Current);
}