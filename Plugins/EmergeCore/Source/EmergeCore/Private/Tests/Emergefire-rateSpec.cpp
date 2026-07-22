// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fire rate (combat/gunplay).
// Converts a weapon's cyclic rate (RPM) into the per-shot interval and gates whether the weapon may fire yet
// (semi/full-auto cadence), plus how many rounds a burst of a given duration produces. Universal firearms math;
// deterministic.
// CONTRACT — NEW class UEmergeFireRate : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeFireRate.h + Private/Combat/EmergeFireRate.cpp, static UFUNCTIONs.
//
//   static float ShotIntervalSec(float Rpm);   // return 60.0f / Rpm;
//   static int32 RoundsInWindow(float Rpm, float WindowSec);   // return (int32)((Rpm / 60.0f) * WindowSec);
//   static bool  CanFire(float LastShotTime, float Now, float Rpm);   // return (Now - LastShotTime) >= ShotIntervalSec(Rpm);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeFireRate.h"

BEGIN_DEFINE_SPEC(FEmergeFireRateSpec, "Emergence.Combat.FireRate",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFireRateSpec)
void FEmergeFireRateSpec::Define()
{
	Describe("ShotIntervalSec & RoundsInWindow", [this]()
	{
		It("RPM to interval and burst count", [this]()
		{
			TestEqual(TEXT("600 rpm -> 0.1s"), UEmergeFireRate::ShotIntervalSec(600.f), 0.1f);
			TestEqual(TEXT("750 rpm -> 0.08s"), UEmergeFireRate::ShotIntervalSec(750.f), 0.08f);
			TestEqual(TEXT("60 rpm -> 1s"), UEmergeFireRate::ShotIntervalSec(60.f), 1.0f);
			TestEqual(TEXT("600rpm in 1s -> 10"), UEmergeFireRate::RoundsInWindow(600.f, 1.0f), 10);
			TestEqual(TEXT("600rpm in 0.5s -> 5"), UEmergeFireRate::RoundsInWindow(600.f, 0.5f), 5);
			TestEqual(TEXT("750rpm in 2s -> 25"), UEmergeFireRate::RoundsInWindow(750.f, 2.0f), 25);
		});
	});

	Describe("CanFire", [this]()
	{
		It("gates on the shot interval having elapsed", [this]()
		{
			TestTrue(TEXT("exactly one interval later"), UEmergeFireRate::CanFire(0.f, 0.1f, 600.f));
			TestFalse(TEXT("too soon"), UEmergeFireRate::CanFire(0.f, 0.05f, 600.f));
			TestTrue(TEXT("well past"), UEmergeFireRate::CanFire(0.f, 1.0f, 600.f));
		});
	});
}
#endif
