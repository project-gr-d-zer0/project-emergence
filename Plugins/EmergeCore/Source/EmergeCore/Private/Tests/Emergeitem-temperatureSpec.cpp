// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: item temperature bands (DayZ-style).
// Classifies an item's temperature (deg C) into the signed level DayZ uses for cooking/burning (hot +1..+4) and
// freezing/spoilage (cold -1..-4), 0 = neutral. Drives cook state, "too hot to hold", and cold-storage logic that
// ties to EmergeFoodSpoilage. Numbers exact from decompiled DayZ constants.c ItemTemperature: HOT 35/70/250/600,
// NEUTRAL 15, COLD 3/-10/-50/-100.
// CONTRACT — NEW class UEmergeItemTemperature : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeItemTemperature.h + Private/Survival/EmergeItemTemperature.cpp, static UFUNCTIONs.
//
//   static int32 TemperatureLevel(float TempC);
//     // if (TempC >= 600.f) return 4;  if (TempC >= 250.f) return 3;  if (TempC >= 70.f) return 2;  if (TempC >= 35.f) return 1;
//     // if (TempC <= -100.f) return -4; if (TempC <= -50.f) return -3; if (TempC <= -10.f) return -2; if (TempC <= 3.f) return -1;
//     // return 0;
//   static bool IsHot(float TempC);    // return TempC >= 35.0f;   // STATE_HOT_LVL_ONE (burn/cook risk)
//   static bool IsFreezing(float TempC); // return TempC <= 3.0f;  // STATE_COLD_LVL_ONE
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeItemTemperature.h"

BEGIN_DEFINE_SPEC(FEmergeItemTemperatureSpec, "Emergence.Survival.ItemTemperature",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeItemTemperatureSpec)
void FEmergeItemTemperatureSpec::Define()
{
	Describe("TemperatureLevel", [this]()
	{
		It("classifies hot +1..+4 at exact thresholds", [this]()
		{
			TestEqual(TEXT("700 -> 4"), UEmergeItemTemperature::TemperatureLevel(700.f), 4);
			TestEqual(TEXT("600 -> 4"), UEmergeItemTemperature::TemperatureLevel(600.f), 4);
			TestEqual(TEXT("250 -> 3"), UEmergeItemTemperature::TemperatureLevel(250.f), 3);
			TestEqual(TEXT("70 -> 2"), UEmergeItemTemperature::TemperatureLevel(70.f), 2);
			TestEqual(TEXT("35 -> 1"), UEmergeItemTemperature::TemperatureLevel(35.f), 1);
		});
		It("neutral band and cold -1..-4", [this]()
		{
			TestEqual(TEXT("20 -> 0 neutral"), UEmergeItemTemperature::TemperatureLevel(20.f), 0);
			TestEqual(TEXT("5 -> 0 neutral (>3)"), UEmergeItemTemperature::TemperatureLevel(5.f), 0);
			TestEqual(TEXT("3 -> -1"), UEmergeItemTemperature::TemperatureLevel(3.f), -1);
			TestEqual(TEXT("-10 -> -2"), UEmergeItemTemperature::TemperatureLevel(-10.f), -2);
			TestEqual(TEXT("-50 -> -3"), UEmergeItemTemperature::TemperatureLevel(-50.f), -3);
			TestEqual(TEXT("-100 -> -4"), UEmergeItemTemperature::TemperatureLevel(-100.f), -4);
			TestEqual(TEXT("-150 -> -4"), UEmergeItemTemperature::TemperatureLevel(-150.f), -4);
		});
	});

	Describe("IsHot / IsFreezing", [this]()
	{
		It("hot at/above 35, freezing at/below 3", [this]()
		{
			TestTrue(TEXT("35 hot"), UEmergeItemTemperature::IsHot(35.f));
			TestFalse(TEXT("34 not hot"), UEmergeItemTemperature::IsHot(34.f));
			TestTrue(TEXT("3 freezing"), UEmergeItemTemperature::IsFreezing(3.f));
			TestFalse(TEXT("4 not freezing"), UEmergeItemTemperature::IsFreezing(4.f));
		});
	});
}
#endif
