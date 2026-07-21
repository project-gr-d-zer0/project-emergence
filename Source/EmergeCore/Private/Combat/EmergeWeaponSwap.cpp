#include "Combat/EmergeWeaponSwap.h"

float UEmergeWeaponSwap::SwapTimeSec(float BaseTime, float WeaponKg)
{
	return BaseTime * (1.0f + WeaponKg * 0.1f);
}

float UEmergeWeaponSwap::FastSwapTimeSec(float BaseTime, float WeaponKg)
{
	return SwapTimeSec(BaseTime, WeaponKg) * 0.6f;
}

bool UEmergeWeaponSwap::CanSwap(bool bReloading)
{
	return !bReloading;
}