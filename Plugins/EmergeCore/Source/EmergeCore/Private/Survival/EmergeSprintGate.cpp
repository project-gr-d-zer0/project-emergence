#include "Survival/EmergeSprintGate.h"

bool UEmergeSprintGate::UpdateExhausted(float Stamina, bool bWasExhausted, float ReArmThreshold)
{
	if (Stamina <= 0.0f)
	{
		return true;
	}
	if (bWasExhausted && Stamina < FMath::Max(ReArmThreshold, 0.0f))
	{
		return true;
	}
	return false;
}
