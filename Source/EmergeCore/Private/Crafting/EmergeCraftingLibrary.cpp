#include "Crafting/EmergeCraftingLibrary.h"

float UEmergeCraftingLibrary::CalculateCompletionTime(float BaseTime, float TierModifier, int32 WorkerCount)
{
	const int32 ClampedWorkers = FMath::Max(1, WorkerCount);
	const float Duration = BaseTime * TierModifier / static_cast<float>(ClampedWorkers);
	return FMath::Max(0.0f, Duration);
}
