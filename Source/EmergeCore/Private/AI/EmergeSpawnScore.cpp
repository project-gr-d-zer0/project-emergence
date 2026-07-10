#include "AI/EmergeSpawnScore.h"

float UEmergeSpawnScore::BehindBias01(float DotWithHeading)
{
	return FMath::Clamp(-DotWithHeading, 0.f, 1.f);
}

float UEmergeSpawnScore::Score(float Threat, float BehindBias, float Presence)
{
	return Threat + 0.75f * BehindBias - 1.25f * Presence;
}

bool UEmergeSpawnScore::IsInViewFrustum(float DotCamToCandidate, float CosHalfFovPlusMargin)
{
	return DotCamToCandidate > CosHalfFovPlusMargin;
}
