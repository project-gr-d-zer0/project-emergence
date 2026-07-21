#include "Combat/EmergeArmorPen.h"

float UEmergeArmorPen::RequiredPen(int32 ArmorClass)
{
	return ArmorClass * 10.0f;
}

bool UEmergeArmorPen::Penetrates(float PenPower, int32 ArmorClass)
{
	return PenPower >= RequiredPen(ArmorClass);
}

float UEmergeArmorPen::DamageMultiplier(float PenPower, int32 ArmorClass)
{
	if (Penetrates(PenPower, ArmorClass))
	{
		return 1.0f;
	}
	return FMath::Clamp(PenPower / RequiredPen(ArmorClass), 0.0f, 1.0f) * 0.3f;
}