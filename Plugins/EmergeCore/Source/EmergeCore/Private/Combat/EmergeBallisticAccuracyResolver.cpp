#include "Combat/EmergeBallisticAccuracyResolver.h"

float UEmergeBallisticAccuracyResolver::ResolveHitChance(float BaseAccuracy, int32 EncumbranceTier, bool bBleeding, bool bStunned)
{
	const float EncM = 1.0f - 0.09f * FMath::Square(static_cast<float>(EncumbranceTier));
	const float VitM = (bBleeding ? 0.85f : 1.0f) * (bStunned ? 0.50f : 1.0f);
	const float Raw = FMath::Clamp(BaseAccuracy * EncM * VitM, 0.0f, 100.0f);
	return FMath::RoundToFloat(Raw * 100.0f) / 100.0f;
}
