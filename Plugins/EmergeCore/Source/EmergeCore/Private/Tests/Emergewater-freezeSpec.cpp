// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: water freeze/thaw (survival).
// Water freezes at/below 0C (can't drink ice) and melts back in warmth at a temperature-scaled rate. Composes
// with EmergeItemTemperature / EmergeWeatherCold. TUNABLE melt rate, fixed deterministic model.
// CONTRACT — NEW class UEmergeWaterFreeze : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeWaterFreeze.h + Private/Survival/EmergeWaterFreeze.cpp, static UFUNCTIONs.
//
//   static bool  IsFrozen(float TempC);   // return TempC <= 0.0f;
//   static bool  CanDrink(bool bFrozen);   // return !bFrozen;
//   static float MeltRatePerTick(float AmbientC, float DeltaT);
//     // return AmbientC > 0.0f ? (AmbientC * 0.1f * DeltaT) : 0.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeWaterFreeze.h"

BEGIN_DEFINE_SPEC(FEmergeWaterFreezeSpec, "Emergence.Survival.WaterFreeze",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWaterFreezeSpec)
void FEmergeWaterFreezeSpec::Define()
{
	Describe("Freeze / thaw", [this]()
	{
		It("frozen at/below 0C, undrinkable; melts in warmth", [this]()
		{
			TestTrue(TEXT("0C frozen"), UEmergeWaterFreeze::IsFrozen(0.f));
			TestTrue(TEXT("-5C frozen"), UEmergeWaterFreeze::IsFrozen(-5.f));
			TestFalse(TEXT("5C liquid"), UEmergeWaterFreeze::IsFrozen(5.f));
			TestFalse(TEXT("can't drink ice"), UEmergeWaterFreeze::CanDrink(true));
			TestTrue(TEXT("can drink liquid"), UEmergeWaterFreeze::CanDrink(false));
			TestEqual(TEXT("10C melts 1.0/s"), UEmergeWaterFreeze::MeltRatePerTick(10.f, 1.f), 1.0f);
			TestEqual(TEXT("freezing -> no melt"), UEmergeWaterFreeze::MeltRatePerTick(0.f, 1.f), 0.0f);
			TestEqual(TEXT("20C dt=2 -> 4.0"), UEmergeWaterFreeze::MeltRatePerTick(20.f, 2.f), 4.0f);
		});
	});
}
#endif
