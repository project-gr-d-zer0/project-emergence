#include "Survival/EmergePainkiller.h"

float UEmergePainkiller::PainReduction(int32 DrugType)
{
	switch (DrugType)
	{
	case 1: return 0.5f;
	case 2: return 1.0f;
	default: return 0.0f;
	}
}

float UEmergePainkiller::DurationSec(int32 DrugType)
{
	switch (DrugType)
	{
	case 1: return 300.0f;
	case 2: return 600.0f;
	default: return 0.0f;
	}
}

bool UEmergePainkiller::SuppressesFracturePain(int32 DrugType)
{
	return DrugType == 2;
}