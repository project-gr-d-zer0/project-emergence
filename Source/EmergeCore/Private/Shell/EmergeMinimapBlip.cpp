#include "Shell/EmergeMinimapBlip.h"

float UEmergeMinimapBlip::WorldToMap01(float WorldCoord, float WorldMin, float WorldMax)
{
	return FMath::Clamp((WorldCoord - WorldMin) / (WorldMax - WorldMin), 0.0f, 1.0f);
}

bool UEmergeMinimapBlip::IsOnMap(float WorldCoord, float WorldMin, float WorldMax)
{
	return WorldCoord >= WorldMin && WorldCoord <= WorldMax;
}