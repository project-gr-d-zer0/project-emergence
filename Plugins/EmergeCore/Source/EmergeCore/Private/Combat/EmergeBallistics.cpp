#include "Combat/EmergeBallistics.h"

bool UEmergeBallistics::Penetrates(float PenetrationValue, int32 ArmorClass)
{
	return PenetratesTuned(FEmergeBallisticsTuning(), PenetrationValue, ArmorClass);
}

float UEmergeBallistics::ComputeDamage(float BaseDamage, float DistanceM, float FalloffStartM, float FalloffEndM)
{
	if (FalloffEndM <= FalloffStartM)
	{
		return BaseDamage;
	}
	if (DistanceM <= FalloffStartM)
	{
		return BaseDamage;
	}
	if (DistanceM >= FalloffEndM)
	{
		return BaseDamage * 0.5f;
	}
	const float T = (DistanceM - FalloffStartM) / (FalloffEndM - FalloffStartM);
	return BaseDamage * (1.0f - 0.5f * T);
}

float UEmergeBallistics::MitigatedDamage(float RawDamage, float ArmorValue, bool bPenetrated)
{
	return MitigatedDamageTuned(FEmergeBallisticsTuning(), RawDamage, ArmorValue, bPenetrated);
}

bool UEmergeBallistics::PenetratesTuned(const FEmergeBallisticsTuning& Tuning, float PenetrationValue, int32 ArmorClass)
{
	return PenetrationValue >= ArmorClass * Tuning.PenPerArmorClass;
}

float UEmergeBallistics::MitigatedDamageTuned(const FEmergeBallisticsTuning& Tuning, float RawDamage, float ArmorValue, bool bPenetrated)
{
	if (!bPenetrated)
	{
		return RawDamage * Tuning.BleedThroughFrac;
	}
	const float Reduce = FMath::Clamp(ArmorValue / Tuning.ArmorReduceDivisor, 0.0f, Tuning.MaxReduceFrac);
	return RawDamage * (1.0f - Reduce);
}
