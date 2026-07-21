#include "Shell/EmergeXpToast.h"

int32 UEmergeXpToast::LevelForXp(int32 Xp, int32 XpPerLevel)
{
	return Xp / XpPerLevel;
}

float UEmergeXpToast::LevelProgress01(int32 Xp, int32 XpPerLevel)
{
	return (float)(Xp % XpPerLevel) / (float)XpPerLevel;
}

bool UEmergeXpToast::DidLevelUp(int32 XpBefore, int32 XpAfter, int32 XpPerLevel)
{
	return LevelForXp(XpAfter, XpPerLevel) > LevelForXp(XpBefore, XpPerLevel);
}