#include "World/EmergeWorldDepthDifficulty.h"

float UEmergeWorldDepthDifficulty::GetDifficultyMultiplier(float Depth) const
{
	if (MaxDepth <= MinDepth)
	{
		return 1.0f;
	}

	const float ClampedDepth = FMath::Clamp(Depth, MinDepth, MaxDepth);
	const float Alpha = (ClampedDepth - MinDepth) / (MaxDepth - MinDepth);
	return 1.0f + Alpha;
}
