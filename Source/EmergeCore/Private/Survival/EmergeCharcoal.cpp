#include "Survival/EmergeCharcoal.h"

float UEmergeCharcoal::FoodPoisonDieOffPerSec(bool bCharcoalActive)
{
	return bCharcoalActive ? 0.5f : 0.0f;
}

float UEmergeCharcoal::AgentReducedPerTick(bool bCharcoalActive, float DeltaT)
{
	return FoodPoisonDieOffPerSec(bCharcoalActive) * DeltaT;
}

float UEmergeCharcoal::TimeToClear(float AgentCount, bool bCharcoalActive)
{
	return bCharcoalActive ? (AgentCount / 0.5f) : -1.0f;   // -1 = never clears on its own
}