#include "Combat/EmergeForegrip.h"

float UEmergeForegrip::RecoilMultiplier(int32 GripType)
{
	switch (GripType)
	{
	case 1: return 0.8f;
	case 2: return 0.85f;
	case 3: return 0.5f;
	default: return 1.0f;
	}
}

float UEmergeForegrip::HandlingMultiplier(int32 GripType)
{
	switch (GripType)
	{
	case 1: return 0.95f;
	case 2: return 1.1f;
	case 3: return 0.7f;
	default: return 1.0f;
	}
}