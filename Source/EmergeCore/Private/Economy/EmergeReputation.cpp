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

int32 UEmergeReputation::TraderTier(int32 PlayerLevel, int32 Rep, int32 LifetimeCommerce)
{
	if (PlayerLevel >= 30 && Rep >= 90 && LifetimeCommerce >= 100000)
	{
		return 3;
	}
	if (PlayerLevel >= 20 && Rep >= 50 && LifetimeCommerce >= 25000)
	{
		return 2;
	}
	if (PlayerLevel >= 10 && Rep >= 0 && LifetimeCommerce >= 5000)
	{
		return 1;
	}
	return 0;
}
