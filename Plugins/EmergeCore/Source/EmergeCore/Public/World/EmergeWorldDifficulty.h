#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWorldDifficulty.generated.h"

// Depth-based difficulty progression: difficulty scales with DEPTH (how far into the
// connected maps you go), not time. Pure, deterministic formulas - first-pass tunable numbers.
UCLASS()
class EMERGECORE_API UEmergeWorldDifficulty : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Horde gets denser the deeper you go: 10 + max(0, Depth) * 15.
	UFUNCTION(BlueprintCallable, Category = "World|Difficulty")
	static int32 HordeDensityAtDepth(int32 Depth);

	// Enemy gear tier steps up with depth, capped at 4.
	// Tier 0 = basic raiders (bolt-action/pistol); tier >= 1 = factions (plate armor, automatics).
	UFUNCTION(BlueprintCallable, Category = "World|Difficulty")
	static int32 EnemyGearTierAtDepth(int32 Depth);

	// Seconds between horde spawns; cadence tightens with depth, floored at 2s.
	UFUNCTION(BlueprintCallable, Category = "World|Difficulty")
	static float HordeSpawnIntervalSeconds(int32 Depth);
};
