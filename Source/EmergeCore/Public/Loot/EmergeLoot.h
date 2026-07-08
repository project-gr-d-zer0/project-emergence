#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeLoot.generated.h"

// Deterministic seeded loot rarity roll. Same (Seed, DepthTier) -> same rarity, always.
// Sparse/low rarity near the entrance (low depth caps rarity), richer deeper.
// Global rarity range is 0..4 (first-pass cap, tunable later).
UCLASS()
class EMERGECORE_API UEmergeLoot : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Rolls a rarity in [0, min(DepthTier, 4)] deterministically from Seed.
	UFUNCTION(BlueprintCallable, Category = "Loot")
	static int32 RollItemRarity(int32 Seed, int32 DepthTier);
};
