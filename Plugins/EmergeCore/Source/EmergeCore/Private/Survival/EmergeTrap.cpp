#include "Survival/EmergeTrap.h"

float UEmergeTrap::CatchChancePerCheck(int32 TrapType, bool bBaited)
{
	float base;
	switch(TrapType)
	{
	case 0: base = 0.2f; break;
	case 1: base = 0.35f; break;
	case 2: base = 0.3f; break;
	default: base = 0.f; break;
	}
	return FMath::Clamp(bBaited ? base * 2.0f : base, 0.0f, 1.0f);
}

bool UEmergeTrap::Caught(float Roll01, float Chance)
{
	return Roll01 < Chance;
}

bool UEmergeTrap::IsValidTrap(int32 TrapType)
{
	return TrapType >= 0 && TrapType <= 2;
}