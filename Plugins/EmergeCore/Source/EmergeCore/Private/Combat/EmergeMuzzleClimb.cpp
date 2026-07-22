#include "Combat/EmergeMuzzleClimb.h"

float UEmergeMuzzleClimb::ClimbAtShot(int32 ShotIndex, float ClimbPerShot, float GripControl01, float MaxClimb)
{
	const float g = FMath::Clamp(GripControl01, 0.0f, 1.0f);
	return FMath::Min(ShotIndex * ClimbPerShot * (1.0f - g), MaxClimb);
}

float UEmergeMuzzleClimb::SettlePerTick(float Current, float SettleRate, float DeltaT)
{
	return FMath::Max(0.0f, Current - SettleRate * DeltaT);
}