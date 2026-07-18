#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeDepthDifficultyCurveResolver.generated.h"

class UEmergeWorldDifficultyConfig;

// Result of sampling the depth difficulty curve at a given depth.
USTRUCT(BlueprintType)
struct EMERGECORE_API FEmergeDifficultySample
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Difficulty")
	float HordeDensityMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Difficulty")
	int32 FactionTierIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Difficulty")
	float LootWeightModifier = 0.5f;
};

// Samples an UEmergeWorldDifficultyConfig at a given depth, producing deterministic
// difficulty curve outputs. No RNG, no time - pure function of (config, depth).
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeDepthDifficultyCurveResolver : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	void Configure(UEmergeWorldDifficultyConfig* InConfig);

	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	FEmergeDifficultySample ResolveAtDepth(float Depth) const;

private:
	UPROPERTY()
	TObjectPtr<UEmergeWorldDifficultyConfig> Config = nullptr;
};
