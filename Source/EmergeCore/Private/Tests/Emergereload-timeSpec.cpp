// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: reload time by ergonomics (combat/gunplay).
// Tarkov-style handling: reload duration scales inversely with weapon ergonomics (higher ergo = faster), and an
// empty-mag reload (that must also chamber a round) takes longer than a tactical reload. TUNABLE design model,
// fixed deterministic.
// CONTRACT — NEW class UEmergeReloadTime : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeReloadTime.h + Private/Combat/EmergeReloadTime.cpp, static UFUNCTIONs.
//
//   static float ReloadTimeSec(float BaseTime, float Ergonomics01, bool bEmptyReload);
//     // const float e = FMath::Clamp(Ergonomics01, 0.0f, 1.0f);
//     // return BaseTime * (2.0f - e) * (bEmptyReload ? 1.3f : 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeReloadTime.h"

BEGIN_DEFINE_SPEC(FEmergeReloadTimeSpec, "Emergence.Combat.ReloadTime",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeReloadTimeSpec)
void FEmergeReloadTimeSpec::Define()
{
	Describe("ReloadTimeSec", [this]()
	{
		It("faster with ergo, slower on empty reload", [this]()
		{
			TestEqual(TEXT("full ergo tactical -> base"), UEmergeReloadTime::ReloadTimeSec(2.0f, 1.0f, false), 2.0f);
			TestEqual(TEXT("zero ergo tactical -> 4.0"), UEmergeReloadTime::ReloadTimeSec(2.0f, 0.0f, false), 4.0f);
			TestEqual(TEXT("half ergo tactical -> 3.0"), UEmergeReloadTime::ReloadTimeSec(2.0f, 0.5f, false), 3.0f);
			TestEqual(TEXT("full ergo empty -> 2.6"), UEmergeReloadTime::ReloadTimeSec(2.0f, 1.0f, true), 2.6f);
			TestEqual(TEXT("ergo clamps above 1"), UEmergeReloadTime::ReloadTimeSec(2.0f, 1.5f, false), 2.0f);
		});
	});
}
#endif
