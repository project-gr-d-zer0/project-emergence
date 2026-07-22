#include "Shell/EmergeNeedIcon.h"

int32 UEmergeNeedIcon::IconState(float Value01)
{
	if (Value01 > 0.6f) return 0;
	if (Value01 > 0.3f) return 1;
	if (Value01 > 0.1f) return 2;
	return 3;
}

bool UEmergeNeedIcon::ShouldFlash(float Value01)
{
	return Value01 <= 0.1f;
}