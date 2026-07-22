#include "Combat/EmergeLimbDamage.h"

float UEmergeLimbDamage::MultiplierForLimb(int32 Limb)
{
	switch (Limb)
	{
	case 0: return 2.5f;
	case 1: return 1.0f;
	case 2: return 1.2f;
	case 3: return 0.7f;
	case 4: return 0.65f;
	default: return 1.0f;
	}
}

float UEmergeLimbDamage::SpeedMultiplierForLegState(int32 State)
{
	switch (State)
	{
	case 0: return 1.0f;
	case 1: return 0.85f;
	case 2: return 0.6f;
	default: return 0.4f;
	}
}

bool UEmergeLimbDamage::CanSprintWithLegState(int32 State)
{
	return State < 2;
}
