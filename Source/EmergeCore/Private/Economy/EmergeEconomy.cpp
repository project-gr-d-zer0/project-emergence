#include "Economy/EmergeEconomy.h"

int32 UEmergeEconomy::ItemValue(int32 BaseValue, float ConditionPct, int32 RarityTier)
{
	const float C = FMath::Clamp(ConditionPct, 0.0f, 100.0f) / 100.0f;
	const float R = 1.0f + FMath::Clamp(RarityTier, 0, 4) * 0.5f;
	return FMath::RoundToInt(BaseValue * C * R);
}

int32 UEmergeEconomy::RepairCost(int32 BaseValue, float FromConditionPct, float ToConditionPct)
{
	const float F = FMath::Clamp(FromConditionPct, 0.0f, 100.0f);
	const float T = FMath::Clamp(ToConditionPct, 0.0f, 100.0f);
	if (T <= F)
	{
		return 0;
	}
	return FMath::RoundToInt(BaseValue * ((T - F) / 100.0f) * 0.5f);
}
