// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: throwable cook timer (combat).
// Grenade fuse logic: once the pin is pulled the fuse burns down; you can "cook" it before throwing, but if the
// remaining fuse is shorter than the throw's flight time it airbursts (or blows in your hand if fully cooked).
// TUNABLE per-throwable fuse, fixed deterministic model.
// CONTRACT — NEW class UEmergeThrowableTimer : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeThrowableTimer.h + Private/Combat/EmergeThrowableTimer.cpp, static UFUNCTIONs.
//
//   static float FuseRemaining(float FuseTime, float Cooked);   // return FMath::Max(0.0f, FuseTime - Cooked);
//   static bool  HasDetonated(float FuseTime, float Cooked);     // return Cooked >= FuseTime;
//   static bool  DetonatesInAir(float FuseTime, float Cooked, float FlightTime);
//     // return FuseRemaining(FuseTime, Cooked) <= FlightTime;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeThrowableTimer.h"

BEGIN_DEFINE_SPEC(FEmergeThrowableTimerSpec, "Emergence.Combat.ThrowableTimer",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeThrowableTimerSpec)
void FEmergeThrowableTimerSpec::Define()
{
	Describe("Fuse burn", [this]()
	{
		It("remaining fuse and detonation", [this]()
		{
			TestEqual(TEXT("5s fuse, 2s cooked -> 3 left"), UEmergeThrowableTimer::FuseRemaining(5.f, 2.f), 3.0f);
			TestEqual(TEXT("fully cooked -> 0 left"), UEmergeThrowableTimer::FuseRemaining(5.f, 5.f), 0.0f);
			TestEqual(TEXT("overcooked clamps 0"), UEmergeThrowableTimer::FuseRemaining(5.f, 7.f), 0.0f);
			TestTrue(TEXT("detonates when cooked to fuse"), UEmergeThrowableTimer::HasDetonated(5.f, 5.f));
			TestFalse(TEXT("not yet detonated"), UEmergeThrowableTimer::HasDetonated(5.f, 4.f));
		});
	});

	Describe("DetonatesInAir", [this]()
	{
		It("airbursts if remaining fuse <= flight time", [this]()
		{
			TestTrue(TEXT("cooked 3, 2 left, 2s flight -> airburst"), UEmergeThrowableTimer::DetonatesInAir(5.f, 3.f, 2.f));
			TestFalse(TEXT("cooked 1, 4 left, 2s flight -> lands first"), UEmergeThrowableTimer::DetonatesInAir(5.f, 1.f, 2.f));
		});
	});
}
#endif
