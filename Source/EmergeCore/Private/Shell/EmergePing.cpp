#include "Shell/EmergePing.h"

float UEmergePing::FadeAlpha(float Elapsed, float Lifetime)
{
	return FMath::Clamp(1.0f - Elapsed / Lifetime, 0.0f, 1.0f);
}

bool UEmergePing::IsExpired(float Elapsed, float Lifetime)
{
	return Elapsed >= Lifetime;
}

float UEmergePing::DistanceScale(float Distance, float RefDistance)
{
	if (Distance <= 0.0f) return 1.0f;
	return FMath::Clamp(RefDistance / Distance, 0.3f, 1.0f);
}