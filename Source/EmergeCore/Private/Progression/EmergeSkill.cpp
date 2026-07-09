#include "Progression/EmergeSkill.h"

namespace
{
	// Cumulative XP thresholds for levels 1..5.
	static const int32 Thresholds[5] = { 75, 225, 525, 1275, 2775 };
}

int32 UEmergeSkill::LevelForXP(int32 XP)
{
	int32 Level = 0;
	for (int32 i = 0; i < 5; ++i)
	{
		if (XP >= Thresholds[i])
		{
			Level = i + 1;
		}
	}
	return Level;
}

int32 UEmergeSkill::XPToNextLevel(int32 XP)
{
	const int32 Level = LevelForXP(XP);
	if (Level >= 5)
	{
		return 0;
	}
	return Thresholds[Level] - XP;
}
