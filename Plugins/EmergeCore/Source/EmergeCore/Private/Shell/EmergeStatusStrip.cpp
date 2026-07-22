#include "Shell/EmergeStatusStrip.h"

float UEmergeStatusStrip::TimeRemaining01(float Elapsed, float Duration)
{
	return FMath::Clamp(1.0f - Elapsed / Duration, 0.0f, 1.0f);
}

bool UEmergeStatusStrip::IsExpired(float Elapsed, float Duration)
{
	return Elapsed >= Duration;
}

bool UEmergeStatusStrip::ShouldBlink(float Elapsed, float Duration, float BlinkThreshold01)
{
	return TimeRemaining01(Elapsed, Duration) <= BlinkThreshold01;
}