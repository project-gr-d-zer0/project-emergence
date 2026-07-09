// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: body temperature (DayZ-style survival).
// Body temp drifts toward danger: cold ambient + wetness chill you; heat sources warm you. Out-of-band
// temperature will feed health/status drains (wired later). Comfort band centers on 37 C.
// CONTRACT — NEW class UEmergeTemperature : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeTemperature.h + Private/Survival/EmergeTemperature.cpp, static UFUNCTIONs:
//   static float BodyTempDeltaPerSec(float AmbientC, float Wetness01, bool bNearHeatSource);
//     // return (AmbientC - 20.0f) * 0.001f - FMath::Clamp(Wetness01, 0.0f, 1.0f) * 0.02f
//     //      + (bNearHeatSource ? 0.05f : 0.0f);
//   static bool IsHypothermic(float BodyTempC);   // return BodyTempC < 35.0f;
//   static bool IsHyperthermic(float BodyTempC);  // return BodyTempC > 39.0f;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeTemperature.h"

BEGIN_DEFINE_SPEC(FEmergeTemperatureSpec, "Emergence.Survival.Temperature",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeTemperatureSpec)
void FEmergeTemperatureSpec::Define()
{
	Describe("BodyTempDeltaPerSec", [this]()
	{
		It("cold + wet chills, heat source warms, exact goldens", [this]()
		{
			TestEqual(TEXT("neutral 20C dry -> 0"), UEmergeTemperature::BodyTempDeltaPerSec(20.f, 0.f, false), 0.f);
			TestEqual(TEXT("0C dry -> -0.02"), UEmergeTemperature::BodyTempDeltaPerSec(0.f, 0.f, false), -0.02f);
			TestEqual(TEXT("0C soaked -> -0.04"), UEmergeTemperature::BodyTempDeltaPerSec(0.f, 1.f, false), -0.04f);
			TestEqual(TEXT("0C soaked + fire -> 0.01"), UEmergeTemperature::BodyTempDeltaPerSec(0.f, 1.f, true), 0.01f);
			TestEqual(TEXT("wetness clamps above 1"), UEmergeTemperature::BodyTempDeltaPerSec(20.f, 5.f, false), -0.02f);
		});
	});

	Describe("Temperature bands", [this]()
	{
		It("hypothermic under 35, hyperthermic over 39", [this]()
		{
			TestTrue(TEXT("34.9 hypo"), UEmergeTemperature::IsHypothermic(34.9f));
			TestFalse(TEXT("35 not hypo"), UEmergeTemperature::IsHypothermic(35.f));
			TestTrue(TEXT("39.1 hyper"), UEmergeTemperature::IsHyperthermic(39.1f));
			TestFalse(TEXT("39 not hyper"), UEmergeTemperature::IsHyperthermic(39.f));
		});
	});
}
#endif
