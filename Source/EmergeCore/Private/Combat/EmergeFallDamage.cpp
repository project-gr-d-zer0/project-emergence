#include "Combat/EmergeFallDamage.h"

float UEmergeFallDamage::DamageFraction(float HeightM, float LowM, float HighM)
{
	if (HeightM <= LowM) return 0.0f;
	if (HeightM >= HighM) return 1.0f;
	return (HeightM - LowM) / (HighM - LowM);
}

float UEmergeFallDamage::HealthDamageFraction(float HeightM)
{
	return DamageFraction(HeightM, 5.0f, 14.0f);
}

float UEmergeFallDamage::ShockDamageFraction(float HeightM)
{
	return DamageFraction(HeightM, 3.0f, 12.0f);
}

bool UEmergeFallDamage::CanBreakLegs(float HeightM)
{
	return HeightM >= 5.0f;
}

bool UEmergeFallDamage::IsSafeFall(float HeightM)
{
	return HeightM < 3.0f;
}