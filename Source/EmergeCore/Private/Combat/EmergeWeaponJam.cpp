#include "Combat/EmergeWeaponJam.h"

float UEmergeWeaponJam::JamChance(float WeaponHealth01, float Heat01)
{
	const float h = FMath::Clamp(WeaponHealth01, 0.0f, 1.0f);
	const float t = FMath::Clamp(Heat01, 0.0f, 1.0f);
	return FMath::Clamp((1.0f - h) * 0.1f + t * 0.05f, 0.0f, 1.0f);
}

bool UEmergeWeaponJam::WouldJam(float JamRoll01, float Chance)
{
	return JamRoll01 < Chance;
}