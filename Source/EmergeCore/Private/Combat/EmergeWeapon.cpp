#include "Combat/EmergeWeapon.h"

float UEmergeWeapon::TimeBetweenShots(int32 RoundsPerMinute)
{
	return 60.0f / FMath::Max(1, RoundsPerMinute);
}

int32 UEmergeWeapon::MagazineRemaining(int32 Loaded, int32 Fired)
{
	return FMath::Max(0, Loaded - FMath::Max(0, Fired));
}

float UEmergeWeapon::RecoilAtShot(float BaseRecoil, int32 ShotIndex)
{
	return BaseRecoil * (1.0f + FMath::Max(0, ShotIndex) * 0.1f);
}

float UEmergeWeapon::HeatAfterShots(float Heat, int32 Shots, float HeatPerShot)
{
	return FMath::Max(0.0f, Heat) + FMath::Max(0, Shots) * FMath::Max(0.0f, HeatPerShot);
}

float UEmergeWeapon::HeatAfterCooling(float Heat, float CoolPerSec, float DeltaSeconds)
{
	return FMath::Max(0.0f, Heat - FMath::Max(0.0f, CoolPerSec) * FMath::Max(0.0f, DeltaSeconds));
}

float UEmergeWeapon::MalfunctionChance(float DurabilityPct, float Heat)
{
	float C = 0.0f;
	if (DurabilityPct < 93.0f)
	{
		C += (93.0f - FMath::Max(0.0f, DurabilityPct)) * 0.001f;
	}
	if (Heat >= 50.0f)
	{
		C += (Heat - 50.0f) * 0.002f;
	}
	return FMath::Clamp(C, 0.0f, 1.0f);
}
