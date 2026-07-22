#include "Nav/EmergeNavPathLen.h"

float UEmergeNavPathLen::RemainingLength(const TArray<FVector>& Points, int32 FromIdx, FVector Current)
{
	if (Points.Num() == 0 || FromIdx >= Points.Num())
	{
		return 0.0f;
	}
	int32 i = FMath::Max(0, FromIdx);
	float Sum = FVector::Dist(Current, Points[i]);
	for (; i + 1 < Points.Num(); ++i)
	{
		Sum += FVector::Dist(Points[i], Points[i + 1]);
	}
	return Sum;
}

float UEmergeNavPathLen::ProgressFraction(float TotalLength, float RemainingLength)
{
	if (TotalLength <= 0.0f)
	{
		return 1.0f;
	}
	return FMath::Clamp(1.0f - RemainingLength / TotalLength, 0.0f, 1.0f);
}
