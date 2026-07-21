#include "Shell/EmergeKillfeed.h"

bool UEmergeKillfeed::IsHeadshotKill(int32 KillFlags)
{
	return (KillFlags & 1) != 0;
}

bool UEmergeKillfeed::IsWallbang(int32 KillFlags)
{
	return (KillFlags & 2) != 0;
}

float UEmergeKillfeed::EntryLifetimeSec(bool bImportant)
{
	return bImportant ? 8.0f : 5.0f;
}