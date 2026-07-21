#include "Combat/EmergeAimPunch.h"

float UEmergeAimPunch::PunchMagnitude(float Damage, float PunchPerDamage, float MaxPunch)
{
	return FMath::Min(Damage * PunchPerDamage, MaxPunch);
}

float UEmergeAimPunch::RecoverPunch(float Current, float RecoverRate, float DeltaT)
{
	return FMath::Max(0.0f, Current - RecoverRate * DeltaT);
}