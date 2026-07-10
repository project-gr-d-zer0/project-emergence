#include "Anim/EmergeStrideSolve.h"

float UEmergeStrideSolve::PlayRate(float SpeedUu, float AuthoredSpeedUu)
{
	return FMath::Clamp(SpeedUu / FMath::Max(AuthoredSpeedUu, 1.f), 1.f, 1.2f);
}

float UEmergeStrideSolve::StrideScale(float SpeedUu, float AuthoredSpeedUu, float InPlayRate)
{
	return FMath::Clamp(SpeedUu / (FMath::Max(AuthoredSpeedUu, 1.f) * FMath::Max(InPlayRate, 0.01f)), 0.9f, 1.35f);
}
