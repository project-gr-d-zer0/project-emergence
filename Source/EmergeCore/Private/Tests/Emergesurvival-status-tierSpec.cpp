// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: generic status/vital tier bands
// (research: bucket continuous vitals into discrete tiers that gate penalties).
// CONTRACT — NEW class UEmergeStatusTier : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeStatusTier.h + Private/Survival/EmergeStatusTier.cpp, static UFUNCTIONs:
//   static int32 TierForValue(float Value);
//     // Value >= 75 -> 0 (fine); 50..74 -> 1 (low); 25..49 -> 2 (bad); < 25 -> 3 (critical)
//   static float PenaltyMultiplierForTier(int32 Tier);
//     // 0 -> 1.0; 1 -> 0.9; 2 -> 0.75; 3+ -> 0.5
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeStatusTier.h"

BEGIN_DEFINE_SPEC(FEmergeStatusTierSpec, "Emergence.Survival.StatusTier",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStatusTierSpec)
void FEmergeStatusTierSpec::Define()
{
	Describe("TierForValue (bands at 75/50/25)", [this]()
	{
		It("matches exact golden values and boundaries", [this]()
		{
			TestEqual(TEXT("100 -> 0"), UEmergeStatusTier::TierForValue(100.f), 0);
			TestEqual(TEXT("75 -> 0"), UEmergeStatusTier::TierForValue(75.f), 0);
			TestEqual(TEXT("74 -> 1"), UEmergeStatusTier::TierForValue(74.f), 1);
			TestEqual(TEXT("50 -> 1"), UEmergeStatusTier::TierForValue(50.f), 1);
			TestEqual(TEXT("49 -> 2"), UEmergeStatusTier::TierForValue(49.f), 2);
			TestEqual(TEXT("24 -> 3"), UEmergeStatusTier::TierForValue(24.f), 3);
		});
	});
	Describe("PenaltyMultiplierForTier", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("t0 -> 1.0"), UEmergeStatusTier::PenaltyMultiplierForTier(0), 1.f);
			TestEqual(TEXT("t1 -> 0.9"), UEmergeStatusTier::PenaltyMultiplierForTier(1), 0.9f);
			TestEqual(TEXT("t2 -> 0.75"), UEmergeStatusTier::PenaltyMultiplierForTier(2), 0.75f);
			TestEqual(TEXT("t3 -> 0.5"), UEmergeStatusTier::PenaltyMultiplierForTier(3), 0.5f);
		});
	});
}
#endif
