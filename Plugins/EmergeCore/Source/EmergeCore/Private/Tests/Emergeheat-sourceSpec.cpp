// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: heat source / fire warmth (DayZ-style).
// A fire warms the player, falling off with distance to zero at its range; standing close counters cold (feeds
// EmergeHeatComfort). The fireplace heating coefficient is exact from decompiled DayZ constants.c
// (TEMP_COEF_FIREPLACE_HEATING 2.0); the distance falloff is a tunable linear model.
// CONTRACT — NEW class UEmergeHeatSource : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeHeatSource.h + Private/Survival/EmergeHeatSource.cpp, static UFUNCTIONs.
//
//   static float HeatAtDistance(float SourceHeat, float Distance, float MaxRange);
//     // if (Distance >= MaxRange) return 0.0f;   return SourceHeat * (1.0f - Distance / MaxRange);
//   static bool  InWarmthRange(float Distance, float MaxRange);   // return Distance < MaxRange;
//   static float FireplaceHeatingRate(float BaseRate);            // return BaseRate * 2.0f; // TEMP_COEF_FIREPLACE_HEATING
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeHeatSource.h"

BEGIN_DEFINE_SPEC(FEmergeHeatSourceSpec, "Emergence.Survival.HeatSource",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHeatSourceSpec)
void FEmergeHeatSourceSpec::Define()
{
	Describe("HeatAtDistance & range", [this]()
	{
		It("linear falloff to zero at max range", [this]()
		{
			TestEqual(TEXT("at source -> full"), UEmergeHeatSource::HeatAtDistance(100.f, 0.f, 10.f), 100.0f);
			TestEqual(TEXT("halfway -> half"), UEmergeHeatSource::HeatAtDistance(100.f, 5.f, 10.f), 50.0f);
			TestEqual(TEXT("at range -> 0"), UEmergeHeatSource::HeatAtDistance(100.f, 10.f, 10.f), 0.0f);
			TestEqual(TEXT("beyond -> 0"), UEmergeHeatSource::HeatAtDistance(100.f, 15.f, 10.f), 0.0f);
			TestTrue(TEXT("inside range warms"), UEmergeHeatSource::InWarmthRange(5.f, 10.f));
			TestFalse(TEXT("at range no warmth"), UEmergeHeatSource::InWarmthRange(10.f, 10.f));
		});
	});

	Describe("FireplaceHeatingRate", [this]()
	{
		It("applies the DayZ fireplace coefficient (2.0)", [this]()
		{
			TestEqual(TEXT("5 -> 10"), UEmergeHeatSource::FireplaceHeatingRate(5.f), 10.0f);
			TestEqual(TEXT("0 -> 0"), UEmergeHeatSource::FireplaceHeatingRate(0.f), 0.0f);
		});
	});
}
#endif
