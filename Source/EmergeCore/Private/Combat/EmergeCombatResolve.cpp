#include "Combat/EmergeBallisticDropoff.h"
#include "Shell/EmergeAmmoType.h"
#include "Combat/EmergeArmorPen.h"
#include "Combat/EmergeHitZone.h"
#include "Combat/EmergeCombatResolve.h"

float UEmergeCombatResolve::ResolveDamage(float BaseDamage, float Distance, float FalloffStart, float FalloffEnd,
	float MinMult, int32 AmmoType, float PenPower, int32 ArmorClass, int32 HitZone)
{
	float d = UEmergeBallisticDropoff::DamageAtRange(BaseDamage, Distance, FalloffStart, FalloffEnd, MinMult);
	d *= UEmergeAmmoType::DamageModifier(AmmoType);
	d *= UEmergeArmorPen::DamageMultiplier(PenPower, ArmorClass);
	d *= UEmergeHitZone::ZoneMultiplier(HitZone);
	return d;
}