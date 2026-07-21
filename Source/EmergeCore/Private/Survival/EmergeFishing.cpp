#include "Survival/EmergeFishing.h"

float UEmergeFishing::BiteChancePerTick(int32 SpotQuality, bool bBaited, float DeltaT)
{
	float b;
	switch(SpotQuality)
	{
		case 0: b = 0.01f; break;
		case 1: b = 0.03f; break;
		case 2: b = 0.06f; break;
		default: b = 0.f;
	}
	if (bBaited) b *= 2.0f;
	return b * DeltaT;
}

bool UEmergeFishing::GotBite(float Roll01, float Chance)
{
	return Roll01 < Chance;
}