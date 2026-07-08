// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: survival vitals (deterministic depletion).
// Core survival math: hydration/health deplete over time and clamp at 0..100; when hydration hits 0 the
// character starves and takes health damage. Pure, deterministic, testable headless (no tick/actor needed).
// CONTRACT — NEW class UEmergeVitals : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeVitals.h + Private/Survival/EmergeVitals.cpp, static UFUNCTIONs:
//   static float Deplete(float Current, float RatePerSec, float DeltaSeconds);
//     // return FMath::Clamp(Current - RatePerSec * DeltaSeconds, 0.0f, 100.0f);
//   static bool  IsStarving(float Hydration);            // return Hydration <= 0.0f
//   static float StarvationDamage(float Hydration, float DeltaSeconds, float DamagePerSec);
//     // return (Hydration <= 0.0f) ? DamagePerSec * DeltaSeconds : 0.0f;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeVitals.h"

BEGIN_DEFINE_SPEC(FEmergeVitalsSpec, "Emergence.Survival.Vitals",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeVitalsSpec)
void FEmergeVitalsSpec::Define()
{
	Describe("Deplete (clamped 0..100)", [this]()
	{
		It("matches exact golden values and clamps at both ends", [this]()
		{
			TestEqual(TEXT("100 - 2*5 -> 90"),         UEmergeVitals::Deplete(100.f, 2.f, 5.f), 90.f);
			TestEqual(TEXT("zero rate -> unchanged"),  UEmergeVitals::Deplete(50.f, 0.f, 10.f), 50.f);
			TestEqual(TEXT("overshoot clamps to 0"),   UEmergeVitals::Deplete(3.f, 1.f, 10.f), 0.f);
			TestEqual(TEXT("far overshoot clamps 0"),  UEmergeVitals::Deplete(100.f, 1.f, 150.f), 0.f);
		});
	});

	Describe("IsStarving (hydration <= 0)", [this]()
	{
		It("is true only at or below zero hydration", [this]()
		{
			TestTrue(TEXT("0 -> starving"),      UEmergeVitals::IsStarving(0.f));
			TestFalse(TEXT("0.1 -> not"),        UEmergeVitals::IsStarving(0.1f));
			TestFalse(TEXT("50 -> not"),         UEmergeVitals::IsStarving(50.f));
		});
	});

	Describe("StarvationDamage (only while starving)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("starving -> 2*10 = 20"),        UEmergeVitals::StarvationDamage(0.f, 10.f, 2.f), 20.f);
			TestEqual(TEXT("hydrated -> no damage"),        UEmergeVitals::StarvationDamage(50.f, 10.f, 2.f), 0.f);
			TestEqual(TEXT("below zero still starving 20"),  UEmergeVitals::StarvationDamage(-5.f, 10.f, 2.f), 20.f);
		});
	});
}
#endif
