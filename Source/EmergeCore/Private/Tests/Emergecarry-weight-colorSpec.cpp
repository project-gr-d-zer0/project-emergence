// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: carry-weight color tier (shell).
// SHELL stratum: the encumbrance bar color - green while light, warming to red as you approach and exceed the
// carry limit. Pairs with EmergeWeightHud (the stamina penalty). Pure view banding; deterministic.
// CONTRACT — NEW class UEmergeCarryWeightColor : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeCarryWeightColor.h + Private/Shell/EmergeCarryWeightColor.cpp, static UFUNCTIONs.
//   Tier int32: 0 green, 1 yellow, 2 orange, 3 red.
//
//   static float WeightFraction01(float Current, float Max);   // return FMath::Clamp(Current / Max, 0.0f, 1.0f);
//   static int32 WeightColorTier(float Fraction01);
//     // if (Fraction01 < 0.5f) return 0;  if (Fraction01 < 0.85f) return 1;  if (Fraction01 < 1.0f) return 2;  return 3;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeCarryWeightColor.h"

BEGIN_DEFINE_SPEC(FEmergeCarryWeightColorSpec, "Emergence.Shell.CarryWeightColor",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCarryWeightColorSpec)
void FEmergeCarryWeightColorSpec::Define()
{
	Describe("WeightFraction01 & WeightColorTier", [this]()
	{
		It("bar fill and color band", [this]()
		{
			TestEqual(TEXT("half loaded"), UEmergeCarryWeightColor::WeightFraction01(50.f, 100.f), 0.5f);
			TestEqual(TEXT("over clamps 1"), UEmergeCarryWeightColor::WeightFraction01(150.f, 100.f), 1.0f);
			TestEqual(TEXT("0.3 green"), UEmergeCarryWeightColor::WeightColorTier(0.3f), 0);
			TestEqual(TEXT("0.5 yellow"), UEmergeCarryWeightColor::WeightColorTier(0.5f), 1);
			TestEqual(TEXT("0.85 orange"), UEmergeCarryWeightColor::WeightColorTier(0.85f), 2);
			TestEqual(TEXT("1.0 red (overloaded)"), UEmergeCarryWeightColor::WeightColorTier(1.0f), 3);
			TestEqual(TEXT("1.2 red"), UEmergeCarryWeightColor::WeightColorTier(1.2f), 3);
		});
	});
}
#endif
