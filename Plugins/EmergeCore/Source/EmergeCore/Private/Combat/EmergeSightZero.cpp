#include "Combat/EmergeSightZero.h"

float UEmergeSightZero::HoldoverAtRange(float Distance, float ZeroDistance, float DropPerMeter)
{
	return (Distance - ZeroDistance) * DropPerMeter;
}

bool UEmergeSightZero::IsZeroedFor(float Distance, float ZeroDistance, float Tolerance)
{
	return FMath::Abs(Distance - ZeroDistance) <= Tolerance;
}