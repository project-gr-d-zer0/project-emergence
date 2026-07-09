#include "Nav/EmergeIntercept.h"

FVector UEmergeIntercept::PredictPosition(FVector TargetPos, FVector TargetVel, float LeadTime)
{
	return TargetPos + TargetVel * FMath::Max(0.0f, LeadTime);
}

float UEmergeIntercept::LeadTime(float Distance, float PursuerSpeed)
{
	return (PursuerSpeed <= 1.0f) ? 0.0f : Distance / PursuerSpeed;
}
