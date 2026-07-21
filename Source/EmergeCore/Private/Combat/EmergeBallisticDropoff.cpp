#include "Combat/EmergeBallisticDropoff.h"

float UEmergeBallisticDropoff::DamageAtRange(float BaseDamage, float Distance, float FalloffStart, float FalloffEnd, float MinMultiplier)
{
	if (Distance <= FalloffStart)
	{
		return BaseDamage;
	}
	if (Distance >= FalloffEnd)
	{
		return BaseDamage * MinMultiplier;
	}
	const float t = (Distance - FalloffStart) / (FalloffEnd - FalloffStart);
	return BaseDamage * FMath::Lerp(1.0f, MinMultiplier, t);
}

float UEmergeBallisticDropoff::VelocityAtRange(float MuzzleVel, float Distance, float DragPerMeter)
{
	return FMath::Max(0.0f, MuzzleVel - DragPerMeter * Distance);
}