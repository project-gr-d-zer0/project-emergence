#include "Sensor/EmergeMemory.h"

float UEmergeMemory::ConfidenceAfter(float LastConf, float DtSeconds, float DecaySeconds)
{
	return FMath::Clamp(LastConf - FMath::Max(0.0f, DtSeconds) / FMath::Max(0.001f, DecaySeconds), 0.0f, 1.0f);
}

bool UEmergeMemory::IsStale(float Confidence, float Floor)
{
	return Confidence < Floor;
}

// [CORPUS-FIX] PZ IsoZombie.java sandbox memory tiers.
float UEmergeMemory::ForgetThreshold(int32 MemoryTier)
{
	switch (MemoryTier)
	{
	case 3: return 1250.0f;  // High
	case 2: return 800.0f;   // Normal
	case 1: return 500.0f;   // Low
	default: return 25.0f;   // None
	}
}

float UEmergeMemory::AccumulateTimeSinceSeen(float Current, float UpdateMultiplier)
{
	return Current + UpdateMultiplier;
}

bool UEmergeMemory::HasForgotten(float TimeSinceSeen, int32 MemoryTier)
{
	return TimeSinceSeen > ForgetThreshold(MemoryTier);
}
