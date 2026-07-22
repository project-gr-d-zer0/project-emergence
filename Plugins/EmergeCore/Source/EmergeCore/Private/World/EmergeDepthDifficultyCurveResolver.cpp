#include "World/EmergeDepthDifficultyCurveResolver.h"
#include "World/EmergeWorldDifficultyConfig.h"

void UEmergeDepthDifficultyCurveResolver::Configure(UEmergeWorldDifficultyConfig* InConfig)
{
	Config = InConfig;
}

FEmergeDifficultySample UEmergeDepthDifficultyCurveResolver::ResolveAtDepth(float Depth) const
{
	FEmergeDifficultySample Sample;

	if (!Config || Config->MaxDifficulty <= 0.0f)
	{
		return Sample;
	}

	const float Alpha = FMath::Clamp(Depth / Config->MaxDifficulty, 0.0f, 1.0f);
	const int32 NumTiers = FMath::Max(1, Config->NumFactionTiers);

	Sample.HordeDensityMultiplier = 1.0f + Alpha * 3.0f;
	Sample.FactionTierIndex = FMath::FloorToInt(Alpha * (NumTiers - 1));
	Sample.LootWeightModifier = 0.5f + Alpha * 1.5f;

	return Sample;
}
