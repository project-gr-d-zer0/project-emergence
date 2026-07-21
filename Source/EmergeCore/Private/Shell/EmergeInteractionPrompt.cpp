#include "Shell/EmergeInteractionPrompt.h"

bool UEmergeInteractionPrompt::InRange(float Distance, float MaxRange)
{
	return Distance <= MaxRange;
}

bool UEmergeInteractionPrompt::IsHigherPriority(float DistA, int32 PrioA, float DistB, int32 PrioB)
{
	if (PrioA != PrioB) return PrioA > PrioB;
	return DistA < DistB;
}

float UEmergeInteractionPrompt::PromptAlpha(float Distance, float MaxRange)
{
	return FMath::Clamp(1.0f - Distance / MaxRange, 0.0f, 1.0f);
}