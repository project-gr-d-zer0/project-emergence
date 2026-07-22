#include "Survival/EmergeBleeding.h"

float UEmergeBleeding::FlowModifier(int32 Intensity)
{
	switch (Intensity)
	{
	case 0: return 0.1f;
	case 1: return 0.6f;
	case 2: return 1.0f;
	default: return 0.f;
	}
}

float UEmergeBleeding::BloodPressureScale(float Blood)
{
	const float s = FMath::Clamp((Blood - 2500.0f) / (4000.0f - 2500.0f), 0.0f, 1.0f);
	return FMath::Clamp(s, 0.3f, 1.0f);
}

float UEmergeBleeding::SourceBloodLossPerTick(int32 Intensity, float Blood, float DeltaT)
{
	return -20.0f * BloodPressureScale(Blood) * DeltaT * FlowModifier(Intensity);
}

float UEmergeBleeding::TotalBloodLossPerTick(int32 NumSources, int32 Intensity, float Blood, float DeltaT)
{
	return NumSources * SourceBloodLossPerTick(Intensity, Blood, DeltaT);
}

int32 UEmergeBleeding::SourceDurationSec()
{
	return 300;
}

float UEmergeBleeding::BurnFlowModifier()
{
	return 0.65f;
}