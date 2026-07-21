// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weather cold rate (survival).
// Bad weather chills the player faster (feeding EmergeTemperature / EmergeHeatComfort); shelter halves it.
// Distinct from EmergeTemperature (the body-temp integrator) - this is the WEATHER-driven cold input.
// TUNABLE per-weather rates, fixed deterministic model.
// CONTRACT — NEW class UEmergeWeatherCold : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeWeatherCold.h + Private/Survival/EmergeWeatherCold.cpp, static UFUNCTIONs.
//   Weather int32: 0 Clear, 1 Rain, 2 Snow, 3 Blizzard.
//
//   static float ColdRatePerTick(int32 Weather, bool bSheltered, float DeltaT);
//     // float r; switch(Weather){ case 1: r=0.02f; break; case 2: r=0.05f; break; case 3: r=0.1f; break; default: r=0.f; }
//     // if (bSheltered) r *= 0.5f;   return r * DeltaT;
//   static bool IsFreezingWeather(int32 Weather);   // return Weather >= 2;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeWeatherCold.h"

BEGIN_DEFINE_SPEC(FEmergeWeatherColdSpec, "Emergence.Survival.WeatherCold",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeatherColdSpec)
void FEmergeWeatherColdSpec::Define()
{
	Describe("ColdRatePerTick", [this]()
	{
		It("worse weather chills faster; shelter halves it", [this]()
		{
			TestEqual(TEXT("clear -> 0"), UEmergeWeatherCold::ColdRatePerTick(0, false, 1.0f), 0.0f);
			TestEqual(TEXT("rain -> 0.02"), UEmergeWeatherCold::ColdRatePerTick(1, false, 1.0f), 0.02f);
			TestEqual(TEXT("snow -> 0.05"), UEmergeWeatherCold::ColdRatePerTick(2, false, 1.0f), 0.05f);
			TestEqual(TEXT("blizzard -> 0.1"), UEmergeWeatherCold::ColdRatePerTick(3, false, 1.0f), 0.1f);
			TestEqual(TEXT("snow sheltered -> 0.025"), UEmergeWeatherCold::ColdRatePerTick(2, true, 1.0f), 0.025f);
			TestEqual(TEXT("blizzard dt=2 -> 0.2"), UEmergeWeatherCold::ColdRatePerTick(3, false, 2.0f), 0.2f);
		});
	});

	Describe("IsFreezingWeather", [this]()
	{
		It("snow and blizzard are freezing", [this]()
		{
			TestFalse(TEXT("rain not freezing"), UEmergeWeatherCold::IsFreezingWeather(1));
			TestTrue(TEXT("snow freezing"), UEmergeWeatherCold::IsFreezingWeather(2));
			TestTrue(TEXT("blizzard freezing"), UEmergeWeatherCold::IsFreezingWeather(3));
		});
	});
}
#endif
