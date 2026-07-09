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

float UEmergeFlee::ScoreFleeCandidate(float EscapeGain, float PathLength, float StraightLineDist, float DetourPenalty)
{
	const float Straight = FMath::Max(StraightLineDist, 1.0f);
	const float Detour = PathLength / Straight;
	return EscapeGain - (Detour - 1.0f) * DetourPenalty;
}
