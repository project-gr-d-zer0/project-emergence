#include "Survival/EmergeCampsite.h"

float UEmergeCampsite::SafetyScore(bool bSheltered, bool bNearWater, int32 ThreatLevel)
{
	float s = 0.5f + (bSheltered ? 0.3f : 0.0f) + (bNearWater ? 0.2f : 0.0f) - ThreatLevel * 0.25f;
	return FMath::Clamp(s, 0.0f, 1.0f);
}

bool UEmergeCampsite::IsSafe(float SafetyScore)
{
	return SafetyScore >= 0.6f;
}