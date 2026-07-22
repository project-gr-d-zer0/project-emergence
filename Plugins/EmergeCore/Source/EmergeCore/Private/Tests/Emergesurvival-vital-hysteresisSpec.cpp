// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: vital severity tiers WITH hysteresis
// (research: DayZ/threshold flapping — worsening is immediate; recovery needs band floor + 5).
// CONTRACT — NEW class UEmergeVitalHysteresis : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeVitalHysteresis.h + Private/Survival/EmergeVitalHysteresis.cpp, static UFUNCTION:
//   static int32 TierStep(int32 CurrentTier, float Value);
//     // Raw bands (same as EmergeStatusTier): >=75 tier0, 50..74 tier1, 25..49 tier2, <25 tier3.
//     // Worsening (raw tier > CurrentTier): return raw tier immediately.
//     // Improving: only move to a better tier N if Value >= FloorOf(N) + 5, where FloorOf: 0->75, 1->50, 2->25.
//     //   Settle at the BEST tier whose floor+5 the value meets; otherwise stay at CurrentTier.
//     // CurrentTier outside 0..3 -> treat as raw tier of Value.
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeVitalHysteresis.h"

BEGIN_DEFINE_SPEC(FEmergeVitalHysteresisSpec, "Emergence.Survival.VitalHysteresis",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeVitalHysteresisSpec)
void FEmergeVitalHysteresisSpec::Define()
{
	Describe("worsening is immediate", [this]()
	{
		It("drops straight to the raw band", [this]()
		{
			TestEqual(TEXT("t0 v74 -> 1"), UEmergeVitalHysteresis::TierStep(0, 74.f), 1);
			TestEqual(TEXT("t1 v24 -> 3"), UEmergeVitalHysteresis::TierStep(1, 24.f), 3);
			TestEqual(TEXT("t0 v10 -> 3"), UEmergeVitalHysteresis::TierStep(0, 10.f), 3);
		});
	});
	Describe("recovery requires floor + 5", [this]()
	{
		It("holds the worse tier inside the hysteresis band", [this]()
		{
			TestEqual(TEXT("t3 v27 -> 3 (needs 30)"), UEmergeVitalHysteresis::TierStep(3, 27.f), 3);
			TestEqual(TEXT("t3 v30 -> 2"), UEmergeVitalHysteresis::TierStep(3, 30.f), 2);
			TestEqual(TEXT("t2 v52 -> 2 (needs 55)"), UEmergeVitalHysteresis::TierStep(2, 52.f), 2);
			TestEqual(TEXT("t2 v55 -> 1"), UEmergeVitalHysteresis::TierStep(2, 55.f), 1);
			TestEqual(TEXT("t1 v78 -> 1 (needs 80)"), UEmergeVitalHysteresis::TierStep(1, 78.f), 1);
			TestEqual(TEXT("t1 v80 -> 0"), UEmergeVitalHysteresis::TierStep(1, 80.f), 0);
		});
		It("settles at the best tier the value clears", [this]()
		{
			TestEqual(TEXT("t3 v80 -> 0 (clears all)"), UEmergeVitalHysteresis::TierStep(3, 80.f), 0);
			TestEqual(TEXT("t3 v60 -> 1 (clears 55, not 80)"), UEmergeVitalHysteresis::TierStep(3, 60.f), 1);
		});
		It("stable when nothing changes", [this]()
		{
			TestEqual(TEXT("t0 v90 -> 0"), UEmergeVitalHysteresis::TierStep(0, 90.f), 0);
			TestEqual(TEXT("t2 v30 -> 2"), UEmergeVitalHysteresis::TierStep(2, 30.f), 2);
		});
	});
}
#endif
