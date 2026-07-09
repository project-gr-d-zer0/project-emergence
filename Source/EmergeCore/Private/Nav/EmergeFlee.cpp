#include "Nav/EmergeFlee.h"

FVector UEmergeFlee::FleeDirection(FVector SelfLocation, FVector Threat)
{
	const FVector D = SelfLocation - Threat;
	return D.IsNearlyZero() ? FVector::ZeroVector : D.GetSafeNormal();
}

FVector UEmergeFlee::FleePoint(FVector SelfLocation, FVector Threat, float Distance)
{
	return SelfLocation + FleeDirection(SelfLocation, Threat) * Distance;
}

float UEmergeFlee::ThreatLevel(float Dist, float DangerRadius)
{
	if (DangerRadius <= 0.0f) return 0.0f;
	return FMath::Clamp(1.0f - Dist / DangerRadius, 0.0f, 1.0f);
}
