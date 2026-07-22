#include "Shell/EmergeStanceIndicator.h"

float UEmergeStanceIndicator::EyeHeightCm(int32 Stance)
{
	switch (Stance)
	{
	case 0: return 170.0f;  // Standing
	case 1: return 100.0f; // Crouched
	case 2: return 30.0f;  // Prone
	default: return 170.0f; // Unknown stance defaults to standing
	}
}

float UEmergeStanceIndicator::MoveSpeedMultiplier(int32 Stance)
{
	switch (Stance)
	{
	case 0: return 1.0f;   // Standing
	case 1: return 0.6f;   // Crouched
	case 2: return 0.25f;  // Prone
	default: return 1.0f; // Unknown stance defaults to standing
	}
}