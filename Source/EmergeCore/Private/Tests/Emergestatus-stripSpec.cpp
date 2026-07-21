// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: buff/debuff strip view-model (shell).
// SHELL stratum: the row of active status-effect icons - each shows time-remaining as a radial fill, drops when
// expired, and blinks when about to run out. Pairs with the existing EmergeStatusEffectComponent/StatusTier sim.
// Pure view math; deterministic.
// CONTRACT — NEW class UEmergeStatusStrip : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeStatusStrip.h + Private/Shell/EmergeStatusStrip.cpp, static UFUNCTIONs.
//
//   static float TimeRemaining01(float Elapsed, float Duration);   // return FMath::Clamp(1.0f - Elapsed / Duration, 0.0f, 1.0f);
//   static bool  IsExpired(float Elapsed, float Duration);          // return Elapsed >= Duration;
//   static bool  ShouldBlink(float Elapsed, float Duration, float BlinkThreshold01);
//     // return TimeRemaining01(Elapsed, Duration) <= BlinkThreshold01;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeStatusStrip.h"

BEGIN_DEFINE_SPEC(FEmergeStatusStripSpec, "Emergence.Shell.StatusStrip",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStatusStripSpec)
void FEmergeStatusStripSpec::Define()
{
	Describe("TimeRemaining01 & IsExpired", [this]()
	{
		It("radial countdown and expiry", [this]()
		{
			TestEqual(TEXT("fresh -> 1"), UEmergeStatusStrip::TimeRemaining01(0.f, 10.f), 1.0f);
			TestEqual(TEXT("half -> 0.5"), UEmergeStatusStrip::TimeRemaining01(5.f, 10.f), 0.5f);
			TestEqual(TEXT("done -> 0"), UEmergeStatusStrip::TimeRemaining01(10.f, 10.f), 0.0f);
			TestEqual(TEXT("overrun clamps 0"), UEmergeStatusStrip::TimeRemaining01(15.f, 10.f), 0.0f);
			TestTrue(TEXT("expired at duration"), UEmergeStatusStrip::IsExpired(10.f, 10.f));
			TestFalse(TEXT("not expired before"), UEmergeStatusStrip::IsExpired(9.f, 10.f));
		});
	});

	Describe("ShouldBlink", [this]()
	{
		It("blinks once remaining drops under the threshold", [this]()
		{
			TestTrue(TEXT("9/10 elapsed, 0.1 rem <= 0.2"), UEmergeStatusStrip::ShouldBlink(9.f, 10.f, 0.2f));
			TestFalse(TEXT("5/10 elapsed, 0.5 rem > 0.2"), UEmergeStatusStrip::ShouldBlink(5.f, 10.f, 0.2f));
		});
	});
}
#endif
