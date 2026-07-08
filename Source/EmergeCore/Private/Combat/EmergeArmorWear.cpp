#include "Combat/EmergeArmorWear.h"

float UEmergeArmorWear::EffectiveProtection(float ArmorValue, float CurDurability, float MaxDurability)
{
	if (MaxDurability <= 0.0f)
	{
		return 0.0f;
	}
	return ArmorValue * FMath::Clamp(CurDurability / MaxDurability, 0.0f, 1.0f);
}

float UEmergeArmorWear::DurabilityLoss(bool bPenetrated, float AmmoArmorDamage)
{
	const float Base = FMath::Max(0.0f, AmmoArmorDamage) * (bPenetrated ? 0.85f : 1.0f);
	return FMath::Max(1.0f, Base);
}

float UEmergeArmorWear::MaxAfterRepair(float MaxDurability)
{
	return FMath::Max(0.0f, MaxDurability * 0.9f);
}
