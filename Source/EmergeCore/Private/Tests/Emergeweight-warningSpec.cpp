// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: equip weight warning (shell).
// SHELL stratum: the encumbrance warning band (none/near-limit/over) and an overloaded flag from carried vs max
// weight. Pairs with EmergeWeightHud / EmergeCarryWeightColor. Pure banding; deterministic.
// CONTRACT — NEW class UEmergeWeightWarning : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeWeightWarning.h + Private/Shell/EmergeWeightWarning.cpp, static UFUNCTIONs.
//   Level int32: 0 None, 1 NearLimit, 2 Over.
//
//   static int32 WarningLevel(float CurrentKg, float MaxKg);
//     // const float f = CurrentKg / MaxKg;   if (f < 0.8f) return 0;   if (f < 1.0f) return 1;   return 2;
//   static bool  IsOverloaded(float CurrentKg, float MaxKg);   // return CurrentKg > MaxKg;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeWeightWarning.h"

BEGIN_DEFINE_SPEC(FEmergeWeightWarningSpec, "Emergence.Shell.WeightWarning",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeightWarningSpec)
void FEmergeWeightWarningSpec::Define()
{
	Describe("WarningLevel & overload", [this]()
	{
		It("bands encumbrance and flags overload", [this]()
		{
			TestEqual(TEXT("half load -> none"), UEmergeWeightWarning::WarningLevel(50.f, 100.f), 0);
			TestEqual(TEXT("0.85 -> near limit"), UEmergeWeightWarning::WarningLevel(85.f, 100.f), 1);
			TestEqual(TEXT("at limit -> over"), UEmergeWeightWarning::WarningLevel(100.f, 100.f), 2);
			TestEqual(TEXT("above limit -> over"), UEmergeWeightWarning::WarningLevel(120.f, 100.f), 2);
			TestTrue(TEXT("120 overloaded"), UEmergeWeightWarning::IsOverloaded(120.f, 100.f));
			TestFalse(TEXT("at limit not over"), UEmergeWeightWarning::IsOverloaded(100.f, 100.f));
			TestFalse(TEXT("under not over"), UEmergeWeightWarning::IsOverloaded(80.f, 100.f));
		});
	});
}
#endif
