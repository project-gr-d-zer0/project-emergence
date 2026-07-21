#include "Survival/EmergeSaline.h"

float UEmergeSaline::BloodRegenPerTick(float DeltaT)
{
	return 3.0f * DeltaT;
}

float UEmergeSaline::WaterRegenPerTick(float DeltaT)
{
	return 2.1f * DeltaT;
}

float UEmergeSaline::LiquidAmount()
{
	return 500.0f;
}

float UEmergeSaline::BloodRestoredOver(float Seconds)
{
	return 3.0f * Seconds;
}