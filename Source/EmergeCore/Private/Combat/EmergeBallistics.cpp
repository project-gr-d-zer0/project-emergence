#include "Combat/EmergeBallistics.h"

bool UEmergeBallistics::Penetrates(float PenetrationValue, int32 ArmorClass)
{
	return PenetrationValue >= ArmorClass * 10.0f;
}

float UEmergeBallistics::ComputeDamage(float BaseDamage, float DistanceM, float FalloffStartM, float FalloffEndM)
{
	if (FalloffEndM <= FalloffStartM)
	{
		return BaseDamage;
	}
	const float T = FMath::Clamp((DistanceM - FalloffStartM) / (FalloffEndM - FalloffStartM), 0.0f, 1.0f);
	return BaseDamage * (1.0f - 0.5f * T);
}
