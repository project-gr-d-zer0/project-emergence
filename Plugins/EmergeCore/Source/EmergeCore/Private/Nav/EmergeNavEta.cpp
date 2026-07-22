#include "Nav/EmergeNavEta.h"

float UEmergeNavEta::EtaSeconds(float RemainingLength, float SpeedUUPerSec)
{
	if (SpeedUUPerSec <= 1.0f)
	{
		return -1.0f;
	}
	return RemainingLength / SpeedUUPerSec;
}

bool UEmergeNavEta::MakingProgress(float PrevDistRemaining, float CurDistRemaining, float MinDelta)
{
	return (PrevDistRemaining - CurDistRemaining) >= MinDelta;
}
