#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmergeWorldDifficultyConfig.generated.h"

// Data-driven config for depth-based difficulty curves (horde density, faction tier,
// loot weight). Content authored as an asset; the resolver samples it deterministically.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeWorldDifficultyConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// Depth at which difficulty reaches its maximum (alpha == 1.0).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty", meta = (ClampMin = "0.0"))
	float MaxDifficulty = 10.0f;

	// Number of distinct faction gear tiers available across the difficulty curve.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Difficulty", meta = (ClampMin = "1"))
	int32 NumFactionTiers = 5;
};
