// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: campfire fuel burn (survival).
// A lit fire consumes fuel over time and dies when it runs out; while lit it is a heat source (EmergeHeatSource)
// and can cook (EmergeCookState). Distinct from those - this is the FUEL economy. TUNABLE burn rate, fixed model.
// CONTRACT — NEW class UEmergeCampfire : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeCampfire.h + Private/Survival/EmergeCampfire.cpp, static UFUNCTIONs.
//
//   static float FuelBurnedPerTick(bool bLit, float DeltaT);   // return bLit ? 0.5f * DeltaT : 0.0f;
//   static float BurnTimeRemaining(float Fuel, float BurnRate);   // return BurnRate > 0.0f ? Fuel / BurnRate : 0.0f;
//   static bool  IsLit(float Fuel);   // return Fuel > 0.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeCampfire.h"

BEGIN_DEFINE_SPEC(FEmergeCampfireSpec, "Emergence.Survival.Campfire",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCampfireSpec)
void FEmergeCampfireSpec::Define()
{
	Describe("Fuel burn", [this]()
	{
		It("consumes fuel while lit and reports burn time", [this]()
		{
			TestEqual(TEXT("lit dt=1 -> 0.5"), UEmergeCampfire::FuelBurnedPerTick(true, 1.0f), 0.5f);
			TestEqual(TEXT("unlit -> 0"), UEmergeCampfire::FuelBurnedPerTick(false, 1.0f), 0.0f);
			TestEqual(TEXT("lit dt=2 -> 1.0"), UEmergeCampfire::FuelBurnedPerTick(true, 2.0f), 1.0f);
			TestEqual(TEXT("100 fuel @0.5/s -> 200s"), UEmergeCampfire::BurnTimeRemaining(100.f, 0.5f), 200.0f);
			TestEqual(TEXT("zero rate -> 0"), UEmergeCampfire::BurnTimeRemaining(100.f, 0.0f), 0.0f);
			TestTrue(TEXT("has fuel -> lit"), UEmergeCampfire::IsLit(50.f));
			TestFalse(TEXT("no fuel -> out"), UEmergeCampfire::IsLit(0.f));
		});
	});
}
#endif
