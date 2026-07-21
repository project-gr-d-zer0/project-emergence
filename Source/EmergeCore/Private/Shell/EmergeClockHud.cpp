#include "Shell/EmergeClockHud.h"

int32 UEmergeClockHud::HourOfDay(float SecondsOfDay)
{
	return ((int32)(SecondsOfDay / 3600.0f)) % 24;
}

int32 UEmergeClockHud::MinuteOfHour(float SecondsOfDay)
{
	return ((int32)(SecondsOfDay / 60.0f)) % 60;
}

bool UEmergeClockHud::IsDaytime(int32 Hour)
{
	return Hour >= 6 && Hour < 18;
}