// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: core ballistics (the differentiator).
// CONTRACT to implement — NEW class UEmergeBallistics : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeBallistics.h + Private/Combat/EmergeBallistics.cpp, with static UFUNCTIONs:
//
//   static bool Penetrates(float PenetrationValue, int32 ArmorClass);
//     // armor stops the round below its rating; formula: return PenetrationValue >= ArmorClass * 10.0f
//
//   static float ComputeDamage(float BaseDamage, float DistanceM, float FalloffStartM, float FalloffEndM);
//     // if FalloffEndM <= FalloffStartM: return BaseDamage (degenerate range guard)
//     // t = clamp((DistanceM - FalloffStartM) / (FalloffEndM - FalloffStartM), 0, 1)
//     // return BaseDamage * (1.0f - 0.5f * t)   // damage decays to 50% at/after FalloffEndM
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeBallistics.h"

BEGIN_DEFINE_SPEC(FEmergeBallisticsSpec, "Emergence.Ballistics.Core",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBallisticsSpec)
void FEmergeBallisticsSpec::Define()
{
	Describe("Penetrates (armor stops round when pen < class*10)", [this]()
	{
		It("matches exact breakpoints", [this]()
		{
			TestTrue(TEXT("pen 45 vs class 4 (>=40) penetrates"),  UEmergeBallistics::Penetrates(45.f, 4));
			TestFalse(TEXT("pen 35 vs class 4 (<40) stopped"),     UEmergeBallistics::Penetrates(35.f, 4));
			TestTrue(TEXT("pen 40 vs class 4 boundary penetrates"),UEmergeBallistics::Penetrates(40.f, 4));
			TestFalse(TEXT("pen 9 vs class 1 (<10) stopped"),      UEmergeBallistics::Penetrates(9.f, 1));
			TestTrue(TEXT("pen 10 vs class 1 boundary penetrates"),UEmergeBallistics::Penetrates(10.f, 1));
		});
	});

	Describe("ComputeDamage (linear falloff to 50% between start and end)", [this]()
	{
		It("matches exact golden values (base=100, start=50, end=150)", [this]()
		{
			TestEqual(TEXT("30m (before start) -> full 100"),   UEmergeBallistics::ComputeDamage(100.f, 30.f, 50.f, 150.f), 100.f);
			TestEqual(TEXT("50m (at start) -> full 100"),       UEmergeBallistics::ComputeDamage(100.f, 50.f, 50.f, 150.f), 100.f);
			TestEqual(TEXT("100m (midpoint) -> 75"),            UEmergeBallistics::ComputeDamage(100.f, 100.f, 50.f, 150.f), 75.f);
			TestEqual(TEXT("150m (at end) -> 50"),              UEmergeBallistics::ComputeDamage(100.f, 150.f, 50.f, 150.f), 50.f);
			TestEqual(TEXT("200m (beyond end) -> clamped 50"),  UEmergeBallistics::ComputeDamage(100.f, 200.f, 50.f, 150.f), 50.f);
		});

		It("guards a degenerate range (end <= start) -> full damage", [this]()
		{
			TestEqual(TEXT("end<=start -> base"), UEmergeBallistics::ComputeDamage(100.f, 80.f, 100.f, 100.f), 100.f);
		});
	});
}
#endif
