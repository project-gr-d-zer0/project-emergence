#include "Survival/EmergePainkiller.h"

// [CORPUS-FIX] DayZ painkillersmdfr.c LIFETIME=240 SetShockMultiplier(0.5); morphinemdfr.c LIFETIME=60
// SetShockMultiplier(0.1). Morphine is the SHORTER, STRONGER one (prior 300/600 was invented + inverted).
float UEmergePainkiller::PainReduction(int32 DrugType)
{
	switch (DrugType)
	{
	case 1: return 0.5f;   // painkiller: shock mult 0.5
	case 2: return 0.9f;   // morphine: shock mult 0.1 -> 0.9 reduction
	default: return 0.0f;
	}
}

float UEmergePainkiller::DurationSec(int32 DrugType)
{
	switch (DrugType)
	{
	case 1: return 240.0f;  // painkiller LIFETIME
	case 2: return 60.0f;   // morphine LIFETIME (shorter)
	default: return 0.0f;
	}
}

bool UEmergePainkiller::SuppressesFracturePain(int32 DrugType)
{
	return DrugType == 2;
}
