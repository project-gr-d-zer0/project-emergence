#include "Survival/EmergeWoundInfection.h"

int32 UEmergeWoundInfection::InfectionStage(int32 WoundAgentCount)
{
	if (WoundAgentCount >= 250) return 2;
	if (WoundAgentCount >= 100) return 1;
	return 0;
}

bool UEmergeWoundInfection::HasFever(int32 WoundAgentCount)
{
	return WoundAgentCount >= 250;
}

float UEmergeWoundInfection::StageTwoDamagePerTick(float DeltaT)
{
	return 0.04f * DeltaT;
}