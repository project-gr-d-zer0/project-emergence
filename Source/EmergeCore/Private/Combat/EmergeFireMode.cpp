#include "Combat/EmergeFireMode.h"

int32 UEmergeFireMode::NextMode(int32 Current, int32 MaxMode)
{
	return (Current + 1) % (MaxMode + 1);
}

int32 UEmergeFireMode::RoundsPerPull(int32 Mode)
{
	switch (Mode)
	{
	case 0: return 0;    // Safe
	case 1: return 1;    // Semi
	case 2: return 3;    // Burst
	case 3: return -1;   // Auto
	default: return 0;
	}
}

bool UEmergeFireMode::CanFire(int32 Mode)
{
	return Mode != 0;    // Not on safe
}