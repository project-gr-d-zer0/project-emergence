#include "Survival/EmergeDrowning.h"

float UEmergeDrowning::SwimmingThreshold()
{
	return 0.4f;
}

float UEmergeDrowning::UnconsciousThreshold()
{
	return 0.1f;
}

bool UEmergeDrowning::IsDrowning(float Submersion01, float Threshold)
{
	return Submersion01 >= Threshold;
}

float UEmergeDrowning::BubbleIntervalMs(float Stamina01)
{
	return FMath::Lerp(1000.0f, 4000.0f, FMath::Clamp(Stamina01, 0.0f, 1.0f));
}