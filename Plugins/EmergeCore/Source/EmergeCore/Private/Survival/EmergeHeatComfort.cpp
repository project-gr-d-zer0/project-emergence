#include "Survival/EmergeHeatComfort.h"

int32 UEmergeHeatComfort::ComfortZone(float HeatComfort)
{
	if (HeatComfort >= 0.75f) return 3;
	if (HeatComfort >= 0.45f) return 2;
	if (HeatComfort >= 0.15f) return 1;
	if (HeatComfort <= -0.75f) return -3;
	if (HeatComfort <= -0.45f) return -2;
	if (HeatComfort <= -0.15f) return -1;
	return 0;
}

float UEmergeHeatComfort::StepTowards(float Current, float Target)
{
	const float Diff = Target - Current;
	if (FMath::Abs(Diff) <= 0.25f) return Target;
	return Current + (Diff > 0.f ? 0.25f : -0.25f);
}

float UEmergeHeatComfort::BodyPartWeight(int32 Slot)
{
	switch (Slot)
	{
	case 0: return 0.05f;  // Head
	case 1: return 0.05f;  // Mask
	case 2: return 0.04f;  // Vest
	case 3: return 0.12f;  // Body
	case 4: return 0.03f;  // Back
	case 5: return 0.04f;  // Gloves
	case 6: return 0.12f;  // Legs
	case 7: return 0.06f;  // Feet
	case 8: return 0.0f;   // Hips
	default: return 0.0f;
	}
}
