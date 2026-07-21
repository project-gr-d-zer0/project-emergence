// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: rain water collection (survival).
// A container gathers water in wet weather at a rate scaled by its opening area, filling toward capacity - a
// renewable water source feeding EmergeMetabolism/thirst. Distinct from EmergeWaterPurity (safety) - this is
// the COLLECTION rate. TUNABLE per-weather rates, fixed deterministic model.
// CONTRACT — NEW class UEmergeRainCollect : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeRainCollect.h + Private/Survival/EmergeRainCollect.cpp, static UFUNCTIONs.
//   Weather int32: 0 Clear, 1 Rain, 2 Snow, 3 Storm.
//
//   static float CollectRatePerTick(int32 Weather, float ContainerArea, float DeltaT);
//     // float r; switch(Weather){ case 1: r=0.5f; break; case 2: r=0.1f; break; case 3: r=1.0f; break; default: r=0.f; }
//     // return r * ContainerArea * DeltaT;
//   static float FilledAfter(float Current, float Rate, float Seconds, float Capacity);
//     // return FMath::Min(Capacity, Current + Rate * Seconds);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeRainCollect.h"

BEGIN_DEFINE_SPEC(FEmergeRainCollectSpec, "Emergence.Survival.RainCollect",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeRainCollectSpec)
void FEmergeRainCollectSpec::Define()
{
	Describe("CollectRatePerTick", [this]()
	{
		It("gathers water in wet weather, scaled by area", [this]()
		{
			TestEqual(TEXT("clear -> 0"), UEmergeRainCollect::CollectRatePerTick(0, 1.f, 1.f), 0.0f);
			TestEqual(TEXT("rain area1 -> 0.5"), UEmergeRainCollect::CollectRatePerTick(1, 1.f, 1.f), 0.5f);
			TestEqual(TEXT("storm area2 -> 2.0"), UEmergeRainCollect::CollectRatePerTick(3, 2.f, 1.f), 2.0f);
			TestEqual(TEXT("snow -> 0.1"), UEmergeRainCollect::CollectRatePerTick(2, 1.f, 1.f), 0.1f);
			TestEqual(TEXT("rain dt=2 -> 1.0"), UEmergeRainCollect::CollectRatePerTick(1, 1.f, 2.f), 1.0f);
		});
	});

	Describe("FilledAfter", [this]()
	{
		It("fills toward capacity, clamped", [this]()
		{
			TestEqual(TEXT("0 + 0.5*100 -> 50"), UEmergeRainCollect::FilledAfter(0.f, 0.5f, 100.f, 100.f), 50.0f);
			TestEqual(TEXT("near full clamps to capacity"), UEmergeRainCollect::FilledAfter(80.f, 0.5f, 100.f, 100.f), 100.0f);
			TestEqual(TEXT("0 + 1*50 -> 50"), UEmergeRainCollect::FilledAfter(0.f, 1.f, 50.f, 100.f), 50.0f);
		});
	});
}
#endif
