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
