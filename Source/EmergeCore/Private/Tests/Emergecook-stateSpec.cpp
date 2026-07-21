// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: cook state (survival cooking).
// Food on heat accumulates cook progress and passes through raw -> cooked -> burned; cooked food is safe/
// nutritious, burned food is wasted. Composes with EmergeItemTemperature (must be hot to cook) and
// EmergeFoodSpoilage. TUNABLE thresholds, fixed deterministic model.
// CONTRACT — NEW class UEmergeCookState : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeCookState.h + Private/Survival/EmergeCookState.cpp, static UFUNCTIONs.
//
//   static int32 CookStage(float CookProgress, float CookedAt, float BurnedAt);
//     // if (CookProgress >= BurnedAt) return 2;  if (CookProgress >= CookedAt) return 1;  return 0;  // 0 raw,1 cooked,2 burned
//   static float CookProgressPerTick(bool bOnHeat, float DeltaT);   // return bOnHeat ? DeltaT : 0.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeCookState.h"

BEGIN_DEFINE_SPEC(FEmergeCookStateSpec, "Emergence.Survival.CookState",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCookStateSpec)
void FEmergeCookStateSpec::Define()
{
	Describe("CookStage", [this]()
	{
		It("raw -> cooked -> burned at thresholds", [this]()
		{
			TestEqual(TEXT("0 raw"), UEmergeCookState::CookStage(0.f, 30.f, 60.f), 0);
			TestEqual(TEXT("29 raw"), UEmergeCookState::CookStage(29.f, 30.f, 60.f), 0);
			TestEqual(TEXT("30 cooked"), UEmergeCookState::CookStage(30.f, 30.f, 60.f), 1);
			TestEqual(TEXT("59 cooked"), UEmergeCookState::CookStage(59.f, 30.f, 60.f), 1);
			TestEqual(TEXT("60 burned"), UEmergeCookState::CookStage(60.f, 30.f, 60.f), 2);
		});
	});

	Describe("CookProgressPerTick", [this]()
	{
		It("advances only while on heat", [this]()
		{
			TestEqual(TEXT("on heat dt=1 -> 1"), UEmergeCookState::CookProgressPerTick(true, 1.0f), 1.0f);
			TestEqual(TEXT("off heat -> 0"), UEmergeCookState::CookProgressPerTick(false, 1.0f), 0.0f);
			TestEqual(TEXT("on heat dt=2 -> 2"), UEmergeCookState::CookProgressPerTick(true, 2.0f), 2.0f);
		});
	});
}
#endif
