#include "Survival/EmergePoisoning.h"
#include "Math/UnrealMathUtility.h"

float UEmergePoisoning::ToxicityDecayPerTick(float DeltaT)
{
	return -1.0f * DeltaT;
}

bool UEmergePoisoning::ShouldVomitFromToxicity(float Toxicity)
{
	return Toxicity > 70.0f;
}

bool UEmergePoisoning::PoisoningActive(int32 FoodPoisonAgentCount)
{
	return FoodPoisonAgentCount >= 1;
}

bool UEmergePoisoning::VomitingActive(int32 FoodPoisonAgentCount)
{
	return FoodPoisonAgentCount > 40;
}

float UEmergePoisoning::InvertedAgentCount(int32 AgentCount, int32 AgentMax)
{
	return FMath::Clamp((float)AgentCount / (float)AgentMax, 0.f, 1.f);
}

float UEmergePoisoning::VomitChancePerTick(int32 AgentCount, int32 AgentMax, float DeltaT)
{
	const float inv = InvertedAgentCount(AgentCount, AgentMax);
	const float occ = FMath::Lerp(2.0f, 5.0f, inv);
	return (occ / 300.0f) * DeltaT;
}

float UEmergePoisoning::VomitDuration(int32 AgentCount, int32 AgentMax)
{
	return FMath::Max(3.0f, 10.0f * InvertedAgentCount(AgentCount, AgentMax));
}

int32 UEmergePoisoning::VomitWaterDrain()
{
	return 70;
}

int32 UEmergePoisoning::VomitEnergyDrain()
{
	return 55;
}

int32 UEmergePoisoning::AgentReducePerVomit()
{
	return 60;
}