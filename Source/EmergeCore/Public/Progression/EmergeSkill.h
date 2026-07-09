#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSkill.generated.h"

// Use-based skill leveling (Project Zomboid style): XP accumulates and levels unlock at
// cumulative banked thresholds. Deterministic and data-free (thresholds are golden values
// per the design spec), capped at level 5.
UCLASS()
class EMERGECORE_API UEmergeSkill : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Returns the highest level whose cumulative threshold <= XP, capped at 5.
	UFUNCTION(BlueprintPure, Category = "Progression")
	static int32 LevelForXP(int32 XP);

	// XP needed to reach the next level (nextThreshold - XP); 0 once at the level cap.
	UFUNCTION(BlueprintPure, Category = "Progression")
	static int32 XPToNextLevel(int32 XP);
};
