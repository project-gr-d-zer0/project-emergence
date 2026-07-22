#include "Nav/EmergeNavSteering.h"

FVector2D UEmergeNavSteering::DesiredHeading(FVector2D From, FVector2D To)
{
	const FVector2D D = To - From;
	return D.IsNearlyZero() ? FVector2D::ZeroVector : D.GetSafeNormal();
}

float UEmergeNavSteering::TurnErrorDeg(FVector2D Heading, FVector2D Facing)
{
	if (Heading.IsNearlyZero() || Facing.IsNearlyZero())
	{
		return 0.0f;
	}
	const float Dot = FMath::Clamp((float)FVector2D::DotProduct(Heading.GetSafeNormal(), Facing.GetSafeNormal()), -1.0f, 1.0f);
	return FMath::RadiansToDegrees(FMath::Acos(Dot));
}

bool UEmergeNavSteering::ShouldSlowForTurn(float TurnErrorDeg, float ThresholdDeg)
{
	return TurnErrorDeg > ThresholdDeg;
}
