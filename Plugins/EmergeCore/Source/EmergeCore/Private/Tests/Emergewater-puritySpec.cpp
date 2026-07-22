// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: water purity (survival).
// Drinking from a source carries a contamination chance (bottled safe, wells low, ponds/stagnant high) that
// seeds disease agents (EmergeDisease/EmergePoisoning); purifying (boil/tablet) removes it. TUNABLE design
// chances, fixed deterministic model.
// CONTRACT — NEW class UEmergeWaterPurity : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeWaterPurity.h + Private/Survival/EmergeWaterPurity.cpp, static UFUNCTIONs.
//   Source int32: 0 Bottled, 1 Well, 2 Pond, 3 Stagnant.
//
//   static float ContaminationChance(int32 Source);
//     // switch: 0 -> 0.0f; 1 -> 0.05f; 2 -> 0.3f; 3 -> 0.6f; default -> 0.0f;
//   static bool  IsSafe(int32 Source);   // return ContaminationChance(Source) <= 0.0f;
//   static float PurifiedChance(float BaseChance, bool bPurified);   // return bPurified ? 0.0f : BaseChance;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeWaterPurity.h"

BEGIN_DEFINE_SPEC(FEmergeWaterPuritySpec, "Emergence.Survival.WaterPurity",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWaterPuritySpec)
void FEmergeWaterPuritySpec::Define()
{
	Describe("ContaminationChance & IsSafe", [this]()
	{
		It("dirtier sources are riskier", [this]()
		{
			TestEqual(TEXT("bottled 0"), UEmergeWaterPurity::ContaminationChance(0), 0.0f);
			TestEqual(TEXT("well 0.05"), UEmergeWaterPurity::ContaminationChance(1), 0.05f);
			TestEqual(TEXT("pond 0.3"), UEmergeWaterPurity::ContaminationChance(2), 0.3f);
			TestEqual(TEXT("stagnant 0.6"), UEmergeWaterPurity::ContaminationChance(3), 0.6f);
			TestTrue(TEXT("bottled safe"), UEmergeWaterPurity::IsSafe(0));
			TestFalse(TEXT("pond not safe"), UEmergeWaterPurity::IsSafe(2));
		});
	});

	Describe("PurifiedChance", [this]()
	{
		It("purification removes contamination risk", [this]()
		{
			TestEqual(TEXT("purified -> 0"), UEmergeWaterPurity::PurifiedChance(0.3f, true), 0.0f);
			TestEqual(TEXT("unpurified -> base"), UEmergeWaterPurity::PurifiedChance(0.3f, false), 0.3f);
		});
	});
}
#endif
