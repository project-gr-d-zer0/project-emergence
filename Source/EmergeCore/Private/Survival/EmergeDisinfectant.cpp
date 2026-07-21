#include "Survival/EmergeDisinfectant.h"

bool UEmergeDisinfectant::CanCure(int32 InfectionStage)
{
	return InfectionStage <= 1;
}

bool UEmergeDisinfectant::RequiresAntibiotics(int32 InfectionStage)
{
	return InfectionStage >= 2;
}

float UEmergeDisinfectant::AgentReductionPerTick(float DeltaT)
{
	return 10.0f * DeltaT;
}