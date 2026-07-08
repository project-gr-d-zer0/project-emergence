// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: deterministic seeded loot rarity roll.
// Canonical design: loot is deterministic per seed (same seed -> same result), sparse/low at the entrance
// (low depth caps rarity), richer deeper. Uses UE's seeded FRandomStream so it is reproducible + testable.
// CONTRACT — NEW class UEmergeLoot : public UBlueprintFunctionLibrary
//   in Public/Loot/EmergeLoot.h + Private/Loot/EmergeLoot.cpp, static UFUNCTION:
//   static int32 RollItemRarity(int32 Seed, int32 DepthTier);
//     // FRandomStream Stream(Seed); return Stream.RandRange(0, FMath::Clamp(DepthTier, 0, 4));
//     // rarity 0..min(DepthTier,4); DETERMINISTIC in (Seed, DepthTier).
// Deterministic + invariant-based (correct pattern for a seeded RNG system): determinism, range, depth-cap,
// and variability are all asserted. First-pass rarity cap of 4; tunable later.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Loot/EmergeLoot.h"
#include "Containers/Set.h"

BEGIN_DEFINE_SPEC(FEmergeLootSpec, "Emergence.Loot.SeededRarity",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeLootSpec)
void FEmergeLootSpec::Define()
{
	Describe("RollItemRarity (deterministic, depth-capped)", [this]()
	{
		It("is DETERMINISTIC: the same seed+tier always yields the same rarity", [this]()
		{
			for (int32 Seed = 0; Seed < 50; ++Seed)
			{
				const int32 A = UEmergeLoot::RollItemRarity(Seed, 4);
				const int32 B = UEmergeLoot::RollItemRarity(Seed, 4);
				TestEqual(TEXT("same seed -> same result"), A, B);
			}
		});

		It("caps rarity at the depth tier (entrance is basic)", [this]()
		{
			for (int32 Seed = 0; Seed < 100; ++Seed)
			{
				TestEqual(TEXT("tier 0 -> only rarity 0"), UEmergeLoot::RollItemRarity(Seed, 0), 0);
				const int32 R2 = UEmergeLoot::RollItemRarity(Seed, 2);
				TestTrue(TEXT("tier 2 -> rarity in [0,2]"), R2 >= 0 && R2 <= 2);
			}
		});

		It("stays within the global 0..4 range even for over-cap tiers", [this]()
		{
			for (int32 Seed = 0; Seed < 100; ++Seed)
			{
				const int32 R = UEmergeLoot::RollItemRarity(Seed, 9);
				TestTrue(TEXT("clamped to [0,4]"), R >= 0 && R <= 4);
			}
		});

		It("is VARIABLE: across seeds at a deep tier it uses more than one rarity", [this]()
		{
			TSet<int32> Seen;
			for (int32 Seed = 0; Seed < 100; ++Seed)
			{
				Seen.Add(UEmergeLoot::RollItemRarity(Seed, 4));
			}
			TestTrue(TEXT("more than one distinct rarity appears"), Seen.Num() > 1);
		});
	});
}
#endif
