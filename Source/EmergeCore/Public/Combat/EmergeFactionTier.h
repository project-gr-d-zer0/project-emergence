#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFactionTier.generated.h"

// Faction/enemy gear tiers by depth: entrance = tier-0 raiders (cloth, bolt-action/pistol,
// small packs); deeper = factions (plate armor, automatic weapons, larger packs).
// Pure, deterministic formulas mapping a gear tier to concrete stats. First-pass balance.
UCLASS()
class EMERGECORE_API UEmergeFactionTier : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Armor value for a gear tier: clamp(tier,0,4) * 20 (0 cloth ... 80 plate).
	UFUNCTION(BlueprintCallable, Category = "Combat|FactionTier")
	static float ArmorValueForTier(int32 Tier);

	// Backpack slots for a gear tier: 12 + clamp(tier,0,4) * 6 (12 ... 36).
	UFUNCTION(BlueprintCallable, Category = "Combat|FactionTier")
	static int32 BackpackSlotsForTier(int32 Tier);

	// Whether this tier uses automatic weapons: tier >= 1 (tier-0 raiders use bolt-action/pistol only).
	UFUNCTION(BlueprintCallable, Category = "Combat|FactionTier")
	static bool UsesAutomaticWeapons(int32 Tier);
};
