#include "Survival/EmergeFracture.h"

float UEmergeFracture::MoveSpeedMultiplier(bool bFractured, bool bSplinted)
{
	if (!bFractured) return 1.0f;
	return bSplinted ? 0.7f : 0.3f;
}

bool UEmergeFracture::CanSprint(bool bFractured, bool bSplinted)
{
	return (!bFractured) || bSplinted;
}

float UEmergeFracture::FractureHealPerTick(bool bSplinted, float DeltaT)
{
	return (bSplinted ? 0.02f : 0.005f) * DeltaT;
}