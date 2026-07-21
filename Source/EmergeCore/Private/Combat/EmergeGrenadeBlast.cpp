#include "Combat/EmergeGrenadeBlast.h"

float UEmergeGrenadeBlast::DamageAtDistance(float MaxDamage, float Distance, float BlastRadius)
{
	if (Distance >= BlastRadius) return 0.0f;
	return MaxDamage * (1.0f - Distance / BlastRadius);
}

bool UEmergeGrenadeBlast::InLethalRange(float Distance, float LethalRadius)
{
	return Distance <= LethalRadius;
}