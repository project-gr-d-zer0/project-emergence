#include "Economy/EmergeReputation.h"

int32 UEmergeReputation::ApplyRepChange(int32 Current, int32 Delta)
{
	return FMath::Clamp(Current + Delta, -100, 100);
}

int32 UEmergeReputation::StandingTier(int32 Rep)
{
	if (Rep < -50)
	{
		return 0;
	}
	if (Rep < 50)
	{
		return 1;
	}
	if (Rep < 90)
	{
		return 2;
	}
	return 3;
}

bool UEmergeReputation::CanAccessSafeZone(int32 Rep)
{
	return Rep >= -50;
}
