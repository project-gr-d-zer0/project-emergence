#include "Survival/EmergeStaminaModel.h"
#include "Math/UnrealMathUtility.h"

float UEmergeStaminaModel::MaxStaminaForLoad(float LoadKg)
{
	return FMath::Max(5.0f, 100.0f - 1.75f * LoadKg);
}

float UEmergeStaminaModel::SprintDrainPerSec(int32 Stance)
{
	switch (Stance)
	{
	case 0: return 4.0f;  // Standing
	case 1: return 1.0f;  // Crouched
	case 2: return 3.0f;  // Prone
	default: return 4.0f; // Unknown -> Standing
	}
}

float UEmergeStaminaModel::ActionDrain(int32 Action)
{
	switch (Action)
	{
	case 0: return 25.0f;  // Jump
	case 1: return 20.0f;  // Vault
	case 2: return 42.0f;  // Climb
	case 3: return 5.0f;   // Roll
	case 4: return 5.0f;   // MeleeLight
	case 5: return 25.0f;  // MeleeHeavy
	case 6: return 8.0f;   // Evade
	default: return 0.0f;  // Unknown
	}
}

float UEmergeStaminaModel::RegenPerSec(int32 MoveState)
{
	switch (MoveState)
	{
	case 0: return 5.0f;  // Idle
	case 1: return 4.0f;  // Walk
	case 2: return 2.0f;  // Jog
	case 3: return 1.0f;  // Swim
	case 4: return 1.0f;  // Ladder
	case 5: return 0.0f;  // Roll
	default: return 0.0f; // Unknown
	}
}