// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: item valuation (barter economy core).
// Ties together durability (condition) + rarity + faction tiers into a tradeable value — the base of the
// player-driven economy ("knowledge curve" differentiator).
// CONTRACT — NEW class UEmergeEconomy : public UBlueprintFunctionLibrary
//   in Public/Economy/EmergeEconomy.h + Private/Economy/EmergeEconomy.cpp, static UFUNCTION:
//   static int32 ItemValue(int32 BaseValue, float ConditionPct, int32 RarityTier);
//     // C = FMath::Clamp(ConditionPct, 0.0f, 100.0f) / 100.0f
//     // R = 1.0f + FMath::Clamp(RarityTier, 0, 4) * 0.5f
//     // return FMath::RoundToInt(BaseValue * C * R);
// Deterministic, exact golden values, boundaries covered (condition + rarity both clamped).
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Economy/EmergeEconomy.h"

BEGIN_DEFINE_SPEC(FEmergeEconomySpec, "Emergence.Economy.ItemValue",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeEconomySpec)
void FEmergeEconomySpec::Define()
{
	Describe("ItemValue (base * condition * rarity-multiplier)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("100 @100% tier0 -> 100"),  UEmergeEconomy::ItemValue(100, 100.f, 0), 100);
			TestEqual(TEXT("100 @50% tier0 -> 50"),     UEmergeEconomy::ItemValue(100, 50.f, 0), 50);
			TestEqual(TEXT("100 @100% tier2 -> 200"),   UEmergeEconomy::ItemValue(100, 100.f, 2), 200);
			TestEqual(TEXT("100 @100% tier4 -> 300"),   UEmergeEconomy::ItemValue(100, 100.f, 4), 300);
			TestEqual(TEXT("200 @100% tier1 -> 300"),   UEmergeEconomy::ItemValue(200, 100.f, 1), 300);
		});

		It("clamps condition and rarity at their boundaries", [this]()
		{
			TestEqual(TEXT("broken (0%) -> 0"),           UEmergeEconomy::ItemValue(100, 0.f, 4), 0);
			TestEqual(TEXT("over-condition clamps 100%"),  UEmergeEconomy::ItemValue(100, 150.f, 0), 100);
			TestEqual(TEXT("over-rarity clamps tier4"),    UEmergeEconomy::ItemValue(100, 100.f, 9), 300);
			TestEqual(TEXT("negative condition -> 0"),     UEmergeEconomy::ItemValue(100, -20.f, 2), 0);
		});
	});
}
#endif
