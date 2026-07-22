#include "Survival/EmergeDisease.h"

float UEmergeDisease::Immunity01(float Energy, float Water, float Health01, float Blood01)
{
	const float e = Energy / 5000.0f;
	const float w = Water / 5000.0f;
	float imm = (e + w + Health01 + Blood01) / 4.0f;
	return FMath::Clamp(imm, 0.0f, 1.0f);
}

int32 UEmergeDisease::ImmunityLevel(float Immunity01)
{
	if (Immunity01 <= 0.15f) return CRITICAL;
	if (Immunity01 <= 0.35f) return LOW;
	if (Immunity01 <= 0.72f) return MEDIUM;
	if (Immunity01 <= 0.95f) return HIGH;
	return GREAT;
}

bool UEmergeDisease::AgentGrows(int32 AgentPotency, int32 ImmunityLevel)
{
	return AgentPotency <= ImmunityLevel;
}

float UEmergeDisease::GrowDelta(float Invasibility, float DieOffSpeed, int32 AgentPotency, int32 ImmunityLevel, float DeltaT)
{
	if (AgentGrows(AgentPotency, ImmunityLevel)) return Invasibility * DeltaT;
	return -DieOffSpeed * DeltaT;
}

float UEmergeDisease::StepCount(float OldCount, float GrowDelta, float MaxCount)
{
	return FMath::Clamp(OldCount + GrowDelta, 0.0f, MaxCount);
}