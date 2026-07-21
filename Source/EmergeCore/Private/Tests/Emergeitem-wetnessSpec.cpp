// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: item wetness weight & rates (DayZ-style).
// Distinct from EmergeWetness (the PLAYER soaking sim): this is how a carried ITEM's wetness scales its weight
// and how fast it wets/dries by context - a soaked coat weighs more (feeding EmergeWeightHud/encumbrance) and
// drips agents. Numbers exact from decompiled DayZ constants.c ItemWetness: WEIGHT DRY/DAMP 1.0, WET 1.33,
// SOAKING 1.66, DRENCHED 2.0; WETTING inside 0.0020 / liquid 0.015; DRYING inside -0.0016 / ground -0.0008.
// CONTRACT — NEW class UEmergeItemWetness : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeItemWetness.h + Private/Survival/EmergeItemWetness.cpp, static UFUNCTIONs.
//   WetnessState int32: 0 Dry, 1 Damp, 2 Wet, 3 SoakingWet, 4 Drenched.
//
//   static float WeightMultiplier(int32 WetnessState);
//     // switch: 2 -> 1.33f; 3 -> 1.66f; 4 -> 2.0f; default (0,1,other) -> 1.0f;
//   static float WettingRatePerTick(bool bInLiquid, float DeltaT);
//     // return (bInLiquid ? 0.015f : 0.0020f) * DeltaT;   // LIQUID vs INSIDE
//   static float DryingRatePerTick(bool bOnGround, float DeltaT);
//     // return (bOnGround ? -0.0008f : -0.0016f) * DeltaT;  // GROUND vs INSIDE (negative = drying)
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeItemWetness.h"

BEGIN_DEFINE_SPEC(FEmergeItemWetnessSpec, "Emergence.Survival.ItemWetness",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeItemWetnessSpec)
void FEmergeItemWetnessSpec::Define()
{
	Describe("WeightMultiplier", [this]()
	{
		It("wet items weigh more (exact DayZ multipliers)", [this]()
		{
			TestEqual(TEXT("dry 1.0"), UEmergeItemWetness::WeightMultiplier(0), 1.0f);
			TestEqual(TEXT("damp 1.0"), UEmergeItemWetness::WeightMultiplier(1), 1.0f);
			TestEqual(TEXT("wet 1.33"), UEmergeItemWetness::WeightMultiplier(2), 1.33f);
			TestEqual(TEXT("soaking 1.66"), UEmergeItemWetness::WeightMultiplier(3), 1.66f);
			TestEqual(TEXT("drenched 2.0"), UEmergeItemWetness::WeightMultiplier(4), 2.0f);
			TestEqual(TEXT("unknown 1.0"), UEmergeItemWetness::WeightMultiplier(9), 1.0f);
		});
	});

	Describe("Wetting & drying rates", [this]()
	{
		It("wetting faster in liquid; drying faster off the ground (per tick)", [this]()
		{
			TestEqual(TEXT("wetting inside dt=1 -> 0.0020"), UEmergeItemWetness::WettingRatePerTick(false, 1.0f), 0.0020f);
			TestEqual(TEXT("wetting liquid dt=1 -> 0.015"), UEmergeItemWetness::WettingRatePerTick(true, 1.0f), 0.015f);
			TestEqual(TEXT("wetting liquid dt=2 -> 0.030"), UEmergeItemWetness::WettingRatePerTick(true, 2.0f), 0.030f);
			TestEqual(TEXT("drying inside dt=1 -> -0.0016"), UEmergeItemWetness::DryingRatePerTick(false, 1.0f), -0.0016f);
			TestEqual(TEXT("drying ground dt=1 -> -0.0008"), UEmergeItemWetness::DryingRatePerTick(true, 1.0f), -0.0008f);
			TestEqual(TEXT("drying inside dt=2 -> -0.0032"), UEmergeItemWetness::DryingRatePerTick(false, 2.0f), -0.0032f);
		});
	});
}
#endif
