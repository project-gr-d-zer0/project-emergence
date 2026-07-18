#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeWorldDepthDifficulty.generated.h"

// Depth-clamped difficulty multiplier: Depth is clamped to [MinDepth, MaxDepth] before
// evaluating the curve, so results never extrapolate past the bound values (no NaN, no throw).
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeWorldDepthDifficulty : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World|Difficulty")
	float MinDepth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World|Difficulty")
	float MaxDepth = 100.0f;

	// Returns 1.0 at MinDepth, 2.0 at MaxDepth, linearly interpolated between; depths
	// outside [MinDepth, MaxDepth] are clamped to the bound before evaluation.
	UFUNCTION(BlueprintCallable, Category = "World|Difficulty")
	float GetDifficultyMultiplier(float Depth) const;
};
