#include "Shell/EmergeMapLegend.h"

int32 UEmergeMapLegend::IconForType(int32 MarkerType)
{
	return (MarkerType >= 0 && MarkerType <= 4) ? MarkerType : 0;
}

bool UEmergeMapLegend::IsHostileMarker(int32 MarkerType)
{
	return MarkerType == 3;
}

float UEmergeMapLegend::IconPriority(int32 MarkerType)
{
	switch (MarkerType)
	{
	case 3: return 3.0f; // enemies draw on top
	case 1: return 2.0f; // objectives next
	default: return 1.0f; // everything else
	}
}