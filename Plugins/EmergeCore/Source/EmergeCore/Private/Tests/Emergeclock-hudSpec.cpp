// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: clock HUD view-model.
// SHELL stratum: the in-game time readout (DayZ/PZ show a watch). Converts seconds-of-day into hour/minute and
// classifies day vs night (used for the watch UI, night-vision prompts, and light/AI-spawn gating). Standard
// wall-clock math; day window 06:00-18:00. Deterministic.
// CONTRACT — NEW class UEmergeClockHud : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeClockHud.h + Private/Shell/EmergeClockHud.cpp, static UFUNCTIONs.
//
//   static int32 HourOfDay(float SecondsOfDay);    // return ((int32)(SecondsOfDay / 3600.0f)) % 24;
//   static int32 MinuteOfHour(float SecondsOfDay);  // return ((int32)(SecondsOfDay / 60.0f)) % 60;
//   static bool  IsDaytime(int32 Hour);             // return Hour >= 6 && Hour < 18;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeClockHud.h"

BEGIN_DEFINE_SPEC(FEmergeClockHudSpec, "Emergence.Shell.ClockHud",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeClockHudSpec)
void FEmergeClockHudSpec::Define()
{
	Describe("HourOfDay & MinuteOfHour", [this]()
	{
		It("converts seconds-of-day into wall clock, wrapping at 24h", [this]()
		{
			TestEqual(TEXT("0s -> 0h"), UEmergeClockHud::HourOfDay(0.f), 0);
			TestEqual(TEXT("43200s -> 12h"), UEmergeClockHud::HourOfDay(43200.f), 12);
			TestEqual(TEXT("3661s -> 1h"), UEmergeClockHud::HourOfDay(3661.f), 1);
			TestEqual(TEXT("86400s wraps -> 0h"), UEmergeClockHud::HourOfDay(86400.f), 0);
			TestEqual(TEXT("0s -> 0m"), UEmergeClockHud::MinuteOfHour(0.f), 0);
			TestEqual(TEXT("90s -> 1m"), UEmergeClockHud::MinuteOfHour(90.f), 1);
			TestEqual(TEXT("3661s -> 1m"), UEmergeClockHud::MinuteOfHour(3661.f), 1);
			TestEqual(TEXT("3600s -> 0m"), UEmergeClockHud::MinuteOfHour(3600.f), 0);
		});
	});

	Describe("IsDaytime", [this]()
	{
		It("day window 06:00-18:00", [this]()
		{
			TestTrue(TEXT("06 day"), UEmergeClockHud::IsDaytime(6));
			TestTrue(TEXT("12 day"), UEmergeClockHud::IsDaytime(12));
			TestTrue(TEXT("17 day"), UEmergeClockHud::IsDaytime(17));
			TestFalse(TEXT("18 night"), UEmergeClockHud::IsDaytime(18));
			TestFalse(TEXT("05 night"), UEmergeClockHud::IsDaytime(5));
			TestFalse(TEXT("00 night"), UEmergeClockHud::IsDaytime(0));
		});
	});
}
#endif
