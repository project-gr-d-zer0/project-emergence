#include "Sensor/EmergeMemory.h"

float UEmergeMemory::ConfidenceAfter(float LastConf, float DtSeconds, float DecaySeconds)
{
	return FMath::Clamp(LastConf - FMath::Max(0.0f, DtSeconds) / FMath::Max(0.001f, DecaySeconds), 0.0f, 1.0f);
}

bool UEmergeMemory::IsStale(float Confidence, float Floor)
{
	return Confidence < Floor;
}
