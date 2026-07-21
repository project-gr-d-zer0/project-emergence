// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: salt preservation (survival).
// Salting food slows its spoilage - a preservation multiplier on the EmergeFoodSpoilage rate. TUNABLE factor,
// fixed deterministic model. Distinct from EmergeFoodSpoilage (the base rate) - this is the salt modifier.
// CONTRACT — NEW class UEmergeSaltPreserve : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeSaltPreserve.h + Private/Survival/EmergeSaltPreserve.cpp, static UFUNCTIONs.
//
//   static float SpoilRateMultiplier(bool bSalted);   // return bSalted ? 0.3f : 1.0f;
//   static float PreservedSpoilPerTick(float BaseSpoilRate, bool bSalted, float DeltaT);
//     // return BaseSpoilRate * SpoilRateMultiplier(bSalted) * DeltaT;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeSaltPreserve.h"

BEGIN_DEFINE_SPEC(FEmergeSaltPreserveSpec, "Emergence.Survival.SaltPreserve",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSaltPreserveSpec)
void FEmergeSaltPreserveSpec::Define()
{
	Describe("Salt preservation", [this]()
	{
		It("salting cuts the spoil rate to 30%", [this]()
		{
			TestEqual(TEXT("salted 0.3x"), UEmergeSaltPreserve::SpoilRateMultiplier(true), 0.3f);
			TestEqual(TEXT("unsalted 1.0x"), UEmergeSaltPreserve::SpoilRateMultiplier(false), 1.0f);
			TestEqual(TEXT("salted spoil dt=1 -> 0.3"), UEmergeSaltPreserve::PreservedSpoilPerTick(1.0f, true, 1.0f), 0.3f);
			TestEqual(TEXT("unsalted spoil dt=1 -> 1.0"), UEmergeSaltPreserve::PreservedSpoilPerTick(1.0f, false, 1.0f), 1.0f);
			TestEqual(TEXT("salted rate 2 dt=1 -> 0.6"), UEmergeSaltPreserve::PreservedSpoilPerTick(2.0f, true, 1.0f), 0.6f);
		});
	});
}
#endif
