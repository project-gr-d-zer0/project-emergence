#include "Combat/EmergeRecoil.h"
#include "Math/UnrealMathUtility.h"

float UEmergeRecoil::VerticalKick(int32 ShotIndex, float BasePitch, float ClimbPerShot, float MaxPitch)
{
	return FMath::Min(BasePitch + ShotIndex * ClimbPerShot, MaxPitch);
}

float UEmergeRecoil::HorizontalKick(int32 ShotIndex, float BaseYaw)
{
	return (ShotIndex % 2 == 0) ? BaseYaw : -BaseYaw;
}

float UEmergeRecoil::RecoverToward(float Current, float RecoverStep)
{
	if (FMath::Abs(Current) <= RecoverStep)
	{
		return 0.0f;
	}
	return Current - (Current > 0.0f ? RecoverStep : -RecoverStep);
}