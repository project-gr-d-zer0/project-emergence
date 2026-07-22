#include "Survival/EmergeDigestion.h"

float UEmergeDigestion::DigestionPerTick(float DeltaT)
{
	return 1.7f * DeltaT;
}

float UEmergeDigestion::SolidEmptiedPerTick(float DeltaT)
{
	return 7.0f * DeltaT;
}

float UEmergeDigestion::EnergyTransferPerTick(float DeltaT)
{
	return 3.0f * DeltaT;
}

float UEmergeDigestion::WaterTransferPerTick(float DeltaT)
{
	return 6.0f * DeltaT;
}

int32 UEmergeDigestion::FullnessLevel(float StomachVolume)
{
	if (StomachVolume >= 1000.0f) return 3;
	if (StomachVolume >= 750.0f) return 2;
	if (StomachVolume >= 1.0f) return 1;
	return 0;
}

float UEmergeDigestion::AgentsTransferred(float DigestedAmount, float Digestibility)
{
	return DigestedAmount * Digestibility;
}