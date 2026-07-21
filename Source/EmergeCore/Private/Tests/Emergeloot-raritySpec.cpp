// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: loot rarity color (shell).
// SHELL stratum: maps an item's value to a rarity tier (common..legendary) and a border-glow strength for the
// inventory/loot UI. TUNABLE value thresholds, fixed deterministic model.
// CONTRACT — NEW class UEmergeLootRarity : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeLootRarity.h + Private/Shell/EmergeLootRarity.cpp, static UFUNCTIONs.
//   Tier: 0 Common, 1 Uncommon, 2 Rare, 3 Epic, 4 Legendary.
//
//   static int32 RarityTier(int32 Value);
//     // if (Value >= 10000) return 4;  if (Value >= 2000) return 3;  if (Value >= 500) return 2;  if (Value >= 100) return 1;  return 0;
//   static float BorderGlow(int32 Tier);   // return FMath::Clamp(Tier / 4.0f, 0.0f, 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeLootRarity.h"

BEGIN_DEFINE_SPEC(FEmergeLootRaritySpec, "Emergence.Shell.LootRarity",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeLootRaritySpec)
void FEmergeLootRaritySpec::Define()
{
	Describe("RarityTier", [this]()
	{
		It("value thresholds map to tiers", [this]()
		{
			TestEqual(TEXT("50 common"), UEmergeLootRarity::RarityTier(50), 0);
			TestEqual(TEXT("99 common"), UEmergeLootRarity::RarityTier(99), 0);
			TestEqual(TEXT("100 uncommon"), UEmergeLootRarity::RarityTier(100), 1);
			TestEqual(TEXT("500 rare"), UEmergeLootRarity::RarityTier(500), 2);
			TestEqual(TEXT("2000 epic"), UEmergeLootRarity::RarityTier(2000), 3);
			TestEqual(TEXT("10000 legendary"), UEmergeLootRarity::RarityTier(10000), 4);
		});
	});

	Describe("BorderGlow", [this]()
	{
		It("glow scales with tier", [this]()
		{
			TestEqual(TEXT("common no glow"), UEmergeLootRarity::BorderGlow(0), 0.0f);
			TestEqual(TEXT("rare half glow"), UEmergeLootRarity::BorderGlow(2), 0.5f);
			TestEqual(TEXT("legendary full glow"), UEmergeLootRarity::BorderGlow(4), 1.0f);
		});
	});
}
#endif
