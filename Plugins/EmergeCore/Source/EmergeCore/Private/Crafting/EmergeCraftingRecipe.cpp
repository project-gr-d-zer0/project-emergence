#include "Crafting/EmergeCraftingRecipe.h"
#include "Crafting/EmergeCraftingLibrary.h"

float UEmergeCraftingRecipe::CalculateCompletionTime(float BaseTime, float TierModifier, int32 WorkerCount) const
{
	return UEmergeCraftingLibrary::CalculateCompletionTime(BaseTime, TierModifier, WorkerCount);
}
