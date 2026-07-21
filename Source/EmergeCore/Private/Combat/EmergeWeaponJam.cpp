#include "Combat/EmergeWeaponJam.h"

// [CORPUS-FIX] EFT malfunction (spt globals.json Overheat): bounded MinMalfChance 0.5% .. MaxMalfChance 9%,
// heat contributes only past the OverheatProblemsStart 100 of MaxOverheat 200 border (Heat01 > 0.5).
float UEmergeWeaponJam::JamChance(float WeaponHealth01, float Heat01)
{
	const float wear = 1.0f - FMath::Clamp(WeaponHealth01, 0.0f, 1.0f);
	const float heatOver = FMath::Clamp((FMath::Clamp(Heat01, 0.0f, 1.0f) - 0.5f) / 0.5f, 0.0f, 1.0f);
	const float raw = 0.005f + wear * 0.06f + heatOver * 0.025f;
	return FMath::Clamp(raw, 0.005f, 0.09f);
}

bool UEmergeWeaponJam::WouldJam(float JamRoll01, float Chance)
{
	return JamRoll01 < Chance;
}
