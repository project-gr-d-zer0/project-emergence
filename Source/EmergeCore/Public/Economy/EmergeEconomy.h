#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeEconomy.generated.h"

// Item valuation: ties durability (condition) + rarity into a tradeable value
// for the player-driven barter economy.
UCLASS()
class EMERGECORE_API UEmergeEconomy : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// C = clamp(ConditionPct, 0, 100) / 100
	// R = 1 + clamp(RarityTier, 0, 4) * 0.5
	// return round(BaseValue * C * R)
	UFUNCTION(BlueprintCallable, Category = "Economy")
	static int32 ItemValue(int32 BaseValue, float ConditionPct, int32 RarityTier);

	// Cost to repair condition from FromConditionPct up to ToConditionPct: half the value of the
	// condition restored. Returns 0 if there is no upward restoration (ToConditionPct <= FromConditionPct).
	UFUNCTION(BlueprintCallable, Category = "Economy")
	static int32 RepairCost(int32 BaseValue, float FromConditionPct, float ToConditionPct);
};
