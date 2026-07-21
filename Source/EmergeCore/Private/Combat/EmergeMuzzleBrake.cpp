#include "Combat/EmergeMuzzleBrake.h"

float UEmergeMuzzleBrake::RecoilMultiplier(bool bBrake)
{
	return bBrake ? 0.75f : 1.0f;
}

float UEmergeMuzzleBrake::SideBlastMultiplier(bool bBrake)
{
	return bBrake ? 1.5f : 1.0f;
}