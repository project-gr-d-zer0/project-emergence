#include "Survival/EmergeNoiseEmission.h"

float UEmergeNoiseEmission::NoiseRadius(int32 Action)
{
	switch (Action)
	{
	case 0: return 2.0f;   // Sneak
	case 1: return 5.0f;   // Walk
	case 2: return 15.0f;  // Run
	case 3: return 25.0f;  // Sprint
	case 4: return 100.0f; // Gunshot
	default: return 0.0f;  // Unknown/Idle
	}
}

bool UEmergeNoiseEmission::CanHear(float NoiseRadius, float ListenerDistance)
{
	return ListenerDistance <= NoiseRadius;
}