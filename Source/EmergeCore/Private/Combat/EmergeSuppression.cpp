#include "Combat/EmergeSuppression.h"

float UEmergeSuppression::SuppressionLevel(float MissDistance, float MaxSuppressRange)
{
	return FMath::Clamp(1.0f - MissDistance / MaxSuppressRange, 0.0f, 1.0f);
}

float UEmergeSuppression::AimPenalty(float SuppressionLevel, float MaxPenalty)
{
	return SuppressionLevel * MaxPenalty;
}

float UEmergeSuppression::DecayPerTick(float Current, float DecayRate, float DeltaT)
{
	return FMath::Max(0.0f, Current - DecayRate * DeltaT);
}