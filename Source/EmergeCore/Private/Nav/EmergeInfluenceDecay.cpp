#include "Nav/EmergeInfluenceDecay.h"

float UEmergeInfluenceDecay::DecayExp(float Value, float DtSeconds, float Tau)
{
	if (Tau <= 0.0f)
	{
		return Value;
	}
	return Value * FMath::Exp(-FMath::Max(0.0f, DtSeconds) / Tau);
}

float UEmergeInfluenceDecay::DiffuseCell(float Center, float NeighborAvg, float Rate)
{
	return FMath::Lerp(Center, NeighborAvg, FMath::Clamp(Rate, 0.0f, 1.0f));
}

float UEmergeInfluenceDecay::SearchRadius(float PlayerSpeed, float StalenessSeconds, float BaseRadius, float MaxRadius)
{
	return FMath::Min(MaxRadius, BaseRadius + FMath::Max(0.0f, PlayerSpeed) * FMath::Max(0.0f, StalenessSeconds));
}

bool UEmergeInfluenceDecay::IsDispersed(float PeakConfidence, float Floor)
{
	return PeakConfidence < Floor;
}
