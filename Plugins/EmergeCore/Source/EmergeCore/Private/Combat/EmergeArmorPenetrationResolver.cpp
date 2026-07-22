#include "Combat/EmergeArmorPenetrationResolver.h"
#include "Combat/EmergeProjectileDefinition.h"
#include "Combat/EmergeArmorItem.h"

float UEmergeArmorPenetrationResolver::ResolveDamage(UEmergeProjectileDefinition* Projectile, UEmergeArmorItem* Armor, float ZoneMultiplier) const
{
	if (!Projectile)
	{
		return 0.0f;
	}

	const float ZoneDamage = Projectile->BaseDamage * ZoneMultiplier;

	if (!Armor)
	{
		return FMath::Max(0.0f, ZoneDamage);
	}

	const bool bPenetrated = Projectile->Penetration >= Armor->ResistanceTier;
	const float Remaining = bPenetrated ? ZoneDamage : ZoneDamage * (1.0f - Armor->Mitigation);
	return FMath::Max(0.0f, Remaining);
}
