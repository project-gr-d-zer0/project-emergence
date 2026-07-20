#include "Survival/EmergeUtilityGrid.h"

bool UEmergeUtilityGrid::IsUtilityOn(int32 ShutoffDay, int32 NightsSurvived)
{
	return ShutoffDay < 0 || NightsSurvived < ShutoffDay;
}

FIntPoint UEmergeUtilityGrid::ShutoffDayRange(int32 UtilityKind, int32 Preset)
{
	int32 Max = -1;
	switch (Preset)
	{
	case 2: Max = 30; break;
	case 3: Max = 60; break;
	case 4: Max = 180; break;
	case 5: Max = 360; break;
	case 6: Max = 1800; break;
	case 7: Max = 180; break;
	case 8: Max = 360; break;
	default: Max = -1; break;
	}

	if (Max < 0)
	{
		return FIntPoint(-1, -1);
	}

	int32 Min = 0;
	if (Preset == 7)
	{
		Min = 60;
	}
	else if (Preset == 8)
	{
		Min = 180;
	}
	else
	{
		Min = (UtilityKind == 1) ? 14 : 0;
	}

	return FIntPoint(Min, Max);
}

int32 UEmergeUtilityGrid::ResolveShutoffDay(int32 UtilityKind, int32 Preset, float Roll01)
{
	const FIntPoint R = ShutoffDayRange(UtilityKind, Preset);
	if (R.X < 0)
	{
		return -1;
	}
	return R.X + FMath::RoundToInt(FMath::Clamp(Roll01, 0.f, 1.f) * float(R.Y - R.X));
}
