#include "Shell/EmergeHealthVignette.h"

float UEmergeHealthVignette::VignetteIntensity(float Health01)
{
	return FMath::Clamp((0.3f - Health01) / 0.3f, 0.0f, 1.0f);
}

float UEmergeHealthVignette::Desaturation01(float Health01)
{
	return FMath::Clamp((0.5f - Health01) / 0.5f, 0.0f, 1.0f);
}

bool UEmergeHealthVignette::IsCriticalPulse(float Health01)
{
	return Health01 < 0.1f;
}