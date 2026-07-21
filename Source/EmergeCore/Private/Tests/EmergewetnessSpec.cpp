// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: wetness / soaking (DayZ-style).
// Clothing/items accumulate wetness in rain and dry from body heat; wind speeds both; items in a backpack
// wet/dry far slower. Past a threshold clothes are "soaking wet" (insulation fails -> cold penalty, feeding
// EmergeHeatComfort) and weigh more. Numbers exact from decompiled DayZ constants.c: wet +0.01/tick raining,
// dry -0.00005/tick, wind effect 25%, backpack passthrough 0.1x, soaking-wet threshold 0.5, soaked weight 1.66x.
// CONTRACT — NEW class UEmergeWetness : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeWetness.h + Private/Survival/EmergeWetness.cpp, static UFUNCTIONs:
//
//   static float WetDeltaPerTick(bool bRaining, bool bInBackpack, float WindStrength01);
//     // float d = bRaining ? 0.01f : -0.00005f;                 // wet in rain, else slowly dry from body heat
//     // d *= (1.0f + FMath::Clamp(WindStrength01, 0.0f, 1.0f) * 0.25f);   // wind speeds wetting AND drying
//     // if (bInBackpack) d *= 0.1f;                             // backpack items change 10x slower
//     // return d;
//
//   static bool IsSoakingWet(float Wetness);      // return Wetness >= 0.5f;   // STATE_SOAKING_WET
//   static bool WetPenaltyActive(float Wetness);  // return Wetness >= 0.5f;   // ENVIRO_WET_PENALTY: insulation degrades
//   static float SoakedWeightMultiplier(bool bSoaked);   // return bSoaked ? 1.66f : 1.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeWetness.h"

BEGIN_DEFINE_SPEC(FEmergeWetnessSpec, "Emergence.Survival.Wetness",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWetnessSpec)
void FEmergeWetnessSpec::Define()
{
	Describe("WetDeltaPerTick", [this]()
	{
		It("wets in rain, dries otherwise; wind scales magnitude; backpack 10x slower", [this]()
		{
			TestEqual(TEXT("rain no wind -> +0.01"), UEmergeWetness::WetDeltaPerTick(true, false, 0.f), 0.01f);
			TestEqual(TEXT("rain full wind -> +0.0125"), UEmergeWetness::WetDeltaPerTick(true, false, 1.f), 0.0125f);
			TestEqual(TEXT("dry no wind -> -0.00005"), UEmergeWetness::WetDeltaPerTick(false, false, 0.f), -0.00005f);
			TestEqual(TEXT("rain in backpack -> +0.001"), UEmergeWetness::WetDeltaPerTick(true, true, 0.f), 0.001f);
			TestEqual(TEXT("wind clamps above 1"), UEmergeWetness::WetDeltaPerTick(true, false, 5.f), 0.0125f);
		});
	});

	Describe("Thresholds & weight", [this]()
	{
		It("soaking-wet, wet penalty, soaked weight (exact DayZ values)", [this]()
		{
			TestTrue(TEXT("0.5 soaking"), UEmergeWetness::IsSoakingWet(0.5f));
			TestFalse(TEXT("0.49 not soaking"), UEmergeWetness::IsSoakingWet(0.49f));
			TestTrue(TEXT("0.5 penalty active"), UEmergeWetness::WetPenaltyActive(0.5f));
			TestFalse(TEXT("0.4 no penalty"), UEmergeWetness::WetPenaltyActive(0.4f));
			TestEqual(TEXT("soaked weight 1.66x"), UEmergeWetness::SoakedWeightMultiplier(true), 1.66f);
			TestEqual(TEXT("dry weight 1x"), UEmergeWetness::SoakedWeightMultiplier(false), 1.0f);
		});
	});
}
#endif
