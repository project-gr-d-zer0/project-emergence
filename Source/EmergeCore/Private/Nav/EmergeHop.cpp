#include "Nav/EmergeHop.h"

float UEmergeHop::HopVerticalVelocity(float ObstacleHeightUu, float GravityZ, float ClearanceMarginUu)
{
	const float H = FMath::Max(ObstacleHeightUu, 0.0f) + FMath::Max(ClearanceMarginUu, 0.0f);
	const float G = FMath::Abs(GravityZ);
	if (G <= 0.0f || H <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Sqrt(2.0f * G * H);
}

bool UEmergeHop::ShouldHop(float LowTraceHitDist, float TriggerDist, bool bHighTraceClear, bool bOnGround)
{
	return bOnGround && bHighTraceClear && LowTraceHitDist >= 0.0f && LowTraceHitDist <= TriggerDist;
}
