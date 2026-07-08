// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: faction/enemy gear tiers by depth.
// Canonical design: entrance = tier-0 raiders (cloth, bolt-action/pistol, small packs); deeper = factions
// (plate armor, automatic weapons, larger packs). This maps a gear tier to concrete deterministic stats.
// CONTRACT — NEW class UEmergeFactionTier : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeFactionTier.h + Private/Combat/EmergeFactionTier.cpp, static UFUNCTIONs:
//   static float ArmorValueForTier(int32 Tier);    // FMath::Clamp(Tier,0,4) * 20.0f   (0 cloth ... 80 plate)
//   static int32 BackpackSlotsForTier(int32 Tier); // 12 + FMath::Clamp(Tier,0,4) * 6   (12 ... 36)
//   static bool  UsesAutomaticWeapons(int32 Tier);  // Tier >= 1  (tier-0 raiders use bolt-action/pistol only)
// Deterministic, exact golden values, boundaries covered. First-pass balance; tune later.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeFactionTier.h"

BEGIN_DEFINE_SPEC(FEmergeFactionTierSpec, "Emergence.Combat.FactionTier",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFactionTierSpec)
void FEmergeFactionTierSpec::Define()
{
	Describe("ArmorValueForTier (clamp(tier,0,4)*20)", [this]()
	{
		It("matches exact golden values and clamps", [this]()
		{
			TestEqual(TEXT("tier 0 -> 0"),          UEmergeFactionTier::ArmorValueForTier(0), 0.f);
			TestEqual(TEXT("tier 2 -> 40"),         UEmergeFactionTier::ArmorValueForTier(2), 40.f);
			TestEqual(TEXT("tier 4 -> 80 (plate)"), UEmergeFactionTier::ArmorValueForTier(4), 80.f);
			TestEqual(TEXT("tier 9 -> 80 (clamped)"),UEmergeFactionTier::ArmorValueForTier(9), 80.f);
			TestEqual(TEXT("negative -> 0 (clamped)"),UEmergeFactionTier::ArmorValueForTier(-1), 0.f);
		});
	});

	Describe("BackpackSlotsForTier (12 + clamp(tier,0,4)*6)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("tier 0 -> 12"),         UEmergeFactionTier::BackpackSlotsForTier(0), 12);
			TestEqual(TEXT("tier 1 -> 18"),         UEmergeFactionTier::BackpackSlotsForTier(1), 18);
			TestEqual(TEXT("tier 4 -> 36 (cap)"),   UEmergeFactionTier::BackpackSlotsForTier(4), 36);
			TestEqual(TEXT("tier 7 -> 36 (clamped)"),UEmergeFactionTier::BackpackSlotsForTier(7), 36);
		});
	});

	Describe("UsesAutomaticWeapons (tier >= 1)", [this]()
	{
		It("only factions (tier>=1) use automatics", [this]()
		{
			TestFalse(TEXT("tier 0 raiders -> no automatics"), UEmergeFactionTier::UsesAutomaticWeapons(0));
			TestTrue(TEXT("tier 1 -> automatics"),             UEmergeFactionTier::UsesAutomaticWeapons(1));
			TestTrue(TEXT("tier 3 -> automatics"),             UEmergeFactionTier::UsesAutomaticWeapons(3));
		});
	});
}
#endif
