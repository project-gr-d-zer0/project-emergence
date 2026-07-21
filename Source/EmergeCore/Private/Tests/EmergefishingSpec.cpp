// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fishing bite chance (survival).
// Passive food from fishing: a better spot and baited line raise the per-tick bite chance; a passed-in roll
// decides (deterministic, no RNG in asserts). TUNABLE design chances, fixed model.
// CONTRACT — NEW class UEmergeFishing : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeFishing.h + Private/Survival/EmergeFishing.cpp, static UFUNCTIONs.
//   SpotQuality int32: 0 Poor, 1 Ok, 2 Good.
//
//   static float BiteChancePerTick(int32 SpotQuality, bool bBaited, float DeltaT);
//     // float b; switch(SpotQuality){ case 0: b=0.01f; break; case 1: b=0.03f; break; case 2: b=0.06f; break; default: b=0.f; }
//     // if (bBaited) b *= 2.0f;   return b * DeltaT;
//   static bool GotBite(float Roll01, float Chance);   // return Roll01 < Chance;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeFishing.h"

BEGIN_DEFINE_SPEC(FEmergeFishingSpec, "Emergence.Survival.Fishing",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFishingSpec)
void FEmergeFishingSpec::Define()
{
	Describe("BiteChancePerTick", [this]()
	{
		It("better spots and bait raise the bite chance", [this]()
		{
			TestEqual(TEXT("poor unbaited 0.01"), UEmergeFishing::BiteChancePerTick(0, false, 1.0f), 0.01f);
			TestEqual(TEXT("good unbaited 0.06"), UEmergeFishing::BiteChancePerTick(2, false, 1.0f), 0.06f);
			TestEqual(TEXT("good baited 0.12"), UEmergeFishing::BiteChancePerTick(2, true, 1.0f), 0.12f);
			TestEqual(TEXT("ok baited 0.06"), UEmergeFishing::BiteChancePerTick(1, true, 1.0f), 0.06f);
			TestEqual(TEXT("poor dt=2 -> 0.02"), UEmergeFishing::BiteChancePerTick(0, false, 2.0f), 0.02f);
		});
	});

	Describe("GotBite", [this]()
	{
		It("bite when roll is under chance", [this]()
		{
			TestTrue(TEXT("0.01 under 0.06"), UEmergeFishing::GotBite(0.01f, 0.06f));
			TestFalse(TEXT("0.1 over 0.06"), UEmergeFishing::GotBite(0.1f, 0.06f));
		});
	});
}
#endif
