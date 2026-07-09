#include "Survival/EmergeInfection.h"

int32 UEmergeInfection::StageForMinutes(float MinutesSinceWound, bool bWoundClean)
{
	if (bWoundClean)
	{
		return 0;
	}
	if (MinutesSinceWound < 20.0f)
	{
		return 0;
	}
	if (MinutesSinceWound < 40.0f)
	{
		return 1;
	}
	if (MinutesSinceWound < 60.0f)
	{
		return 2;
	}
	return 3;
}

bool UEmergeInfection::ItemCuresStage(int32 ItemType, int32 Stage)
{
	return (Stage == 1 && ItemType == 3) || (Stage == 2 && ItemType == 4);
}

float UEmergeInfection::StaminaRegenMultiplierForStage(int32 Stage)
{
	if (Stage <= 0)
	{
		return 1.0f;
	}
	if (Stage == 1)
	{
		return 0.75f;
	}
	if (Stage == 2)
	{
		return 0.5f;
	}
	return 0.25f;
}
