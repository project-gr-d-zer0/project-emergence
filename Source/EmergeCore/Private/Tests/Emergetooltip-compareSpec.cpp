// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: tooltip stat compare (shell).
// SHELL stratum: the equip-comparison arrows - given a candidate item's stat vs the currently equipped one, is it
// better/worse/equal (respecting stats where lower is better, e.g. weight/ADS-time) and by how much. Pure
// comparison; deterministic.
// CONTRACT — NEW class UEmergeTooltipCompare : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeTooltipCompare.h + Private/Shell/EmergeTooltipCompare.cpp, static UFUNCTIONs.
//
//   static int32 CompareStat(float NewVal, float CurrentVal, bool bHigherBetter);
//     // if (NewVal == CurrentVal) return 0;
//     // const bool bBetter = bHigherBetter ? (NewVal > CurrentVal) : (NewVal < CurrentVal);
//     // return bBetter ? 1 : -1;
//   static float StatDelta(float NewVal, float CurrentVal);   // return NewVal - CurrentVal;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeTooltipCompare.h"

BEGIN_DEFINE_SPEC(FEmergeTooltipCompareSpec, "Emergence.Shell.TooltipCompare",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeTooltipCompareSpec)
void FEmergeTooltipCompareSpec::Define()
{
	Describe("CompareStat", [this]()
	{
		It("better/worse/equal, respecting stat direction", [this]()
		{
			TestEqual(TEXT("higher-better: 10 vs 5 -> better"), UEmergeTooltipCompare::CompareStat(10.f, 5.f, true), 1);
			TestEqual(TEXT("higher-better: 5 vs 10 -> worse"), UEmergeTooltipCompare::CompareStat(5.f, 10.f, true), -1);
			TestEqual(TEXT("equal -> 0"), UEmergeTooltipCompare::CompareStat(5.f, 5.f, true), 0);
			TestEqual(TEXT("lower-better: 5 vs 10 -> better"), UEmergeTooltipCompare::CompareStat(5.f, 10.f, false), 1);
			TestEqual(TEXT("lower-better: 10 vs 5 -> worse"), UEmergeTooltipCompare::CompareStat(10.f, 5.f, false), -1);
		});
	});

	Describe("StatDelta", [this]()
	{
		It("signed difference", [this]()
		{
			TestEqual(TEXT("+5"), UEmergeTooltipCompare::StatDelta(10.f, 5.f), 5.0f);
			TestEqual(TEXT("-5"), UEmergeTooltipCompare::StatDelta(5.f, 10.f), -5.0f);
		});
	});
}
#endif
