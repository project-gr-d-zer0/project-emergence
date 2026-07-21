// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: food portion split (survival).
// Splitting a stack of food/water into portions and tracking what remains after eating - the quantity math behind
// rationing. Feeds EmergeDigestion. Pure arithmetic; deterministic.
// CONTRACT — NEW class UEmergeFoodPortion : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeFoodPortion.h + Private/Survival/EmergeFoodPortion.cpp, static UFUNCTIONs.
//
//   static float PortionSize(float TotalAmount, int32 Portions);
//     // return Portions > 0 ? (TotalAmount / Portions) : TotalAmount;
//   static int32 MaxPortions(float TotalAmount, float MinPortion);
//     // return MinPortion > 0.0f ? (int32)(TotalAmount / MinPortion) : 0;
//   static float Remaining(float TotalAmount, float Consumed);   // return FMath::Max(0.0f, TotalAmount - Consumed);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeFoodPortion.h"

BEGIN_DEFINE_SPEC(FEmergeFoodPortionSpec, "Emergence.Survival.FoodPortion",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFoodPortionSpec)
void FEmergeFoodPortionSpec::Define()
{
	Describe("Portioning", [this]()
	{
		It("split size, max portions, and remainder", [this]()
		{
			TestEqual(TEXT("100 into 4 -> 25"), UEmergeFoodPortion::PortionSize(100.f, 4), 25.0f);
			TestEqual(TEXT("0 portions -> whole"), UEmergeFoodPortion::PortionSize(100.f, 0), 100.0f);
			TestEqual(TEXT("100 at min 25 -> 4 portions"), UEmergeFoodPortion::MaxPortions(100.f, 25.f), 4);
			TestEqual(TEXT("100 at min 30 -> 3 portions"), UEmergeFoodPortion::MaxPortions(100.f, 30.f), 3);
			TestEqual(TEXT("ate 30 of 100 -> 70 left"), UEmergeFoodPortion::Remaining(100.f, 30.f), 70.0f);
			TestEqual(TEXT("over-eat clamps 0"), UEmergeFoodPortion::Remaining(100.f, 120.f), 0.0f);
		});
	});
}
#endif
