#include "Shell/EmergeReticle.h"

int32 UEmergeReticle::ReticleForOptic(int32 OpticType)
{
	switch (OpticType)
	{
	case 0: return 0;
	case 1: return 1;
	case 2: return 2;
	case 3: return 3;
	default: return 0;
	}
}

bool UEmergeReticle::HasRangefinder(int32 OpticType)
{
	return OpticType == 3;
}

float UEmergeReticle::CenterDotSize(int32 OpticType)
{
	switch (OpticType)
	{
	case 1: return 2.0f;
	case 2: return 1.5f;
	default: return 1.0f;
	}
}
