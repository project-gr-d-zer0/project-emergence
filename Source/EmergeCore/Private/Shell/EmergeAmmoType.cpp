#include "Shell/EmergeAmmoType.h"

bool UEmergeAmmoType::IsArmorPiercing(int32 AmmoType)
{
	return AmmoType == 2;
}

int32 UEmergeAmmoType::IconColor(int32 AmmoType)
{
	switch (AmmoType)
	{
	case 0: return 0;
	case 1: return 1;
	case 2: return 2;
	case 3: return 3;
	default: return 0;
	}
}

float UEmergeAmmoType::DamageModifier(int32 AmmoType)
{
	switch (AmmoType)
	{
	case 1: return 1.3f;
	case 2: return 0.9f;
	default: return 1.0f;
	}
}