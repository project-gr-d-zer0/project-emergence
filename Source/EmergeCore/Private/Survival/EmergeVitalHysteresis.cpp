#include "Survival/EmergeVitalHysteresis.h"

int32 UEmergeVitalHysteresis::RawTier(float Value)
{
	if (Value >= 75.0f) { return 0; }
	if (Value >= 50.0f) { return 1; }
	if (Value >= 25.0f) { return 2; }
	return 3;
}

int32 UEmergeVitalHysteresis::TierStep(int32 CurrentTier, float Value)
{
	const int32 Raw = RawTier(Value);

	if (CurrentTier < 0 || CurrentTier > 3)
	{
		return Raw;
	}

	// Worsening is immediate.
	if (Raw > CurrentTier)
	{
		return Raw;
	}

	// Improving: find the best (lowest) tier N strictly better than CurrentTier whose floor + 5
	// is met; floors only exist for tiers 0..2 (tier3 has no floor, it's the worst band).
	static const float Floors[3] = { 75.0f, 50.0f, 25.0f };
	for (int32 N = 0; N < CurrentTier; ++N)
	{
		if (Value >= Floors[N] + 5.0f)
		{
			return N;
		}
	}

	return CurrentTier;
}
