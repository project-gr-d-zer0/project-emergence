// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: campsite safety score (survival).
// Rates a would-be campsite: shelter and nearby water raise it, local threat lowers it - used to decide whether
// it is safe to rest/sleep (EmergeSleepRecovery). TUNABLE weights, fixed deterministic model.
// CONTRACT — NEW class UEmergeCampsite : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeCampsite.h + Private/Survival/EmergeCampsite.cpp, static UFUNCTIONs.
//
//   static float SafetyScore(bool bSheltered, bool bNearWater, int32 ThreatLevel);
//     // float s = 0.5f + (bSheltered ? 0.3f : 0.0f) + (bNearWater ? 0.2f : 0.0f) - ThreatLevel * 0.25f;
//     // return FMath::Clamp(s, 0.0f, 1.0f);
//   static bool IsSafe(float SafetyScore);   // return SafetyScore >= 0.6f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeCampsite.h"

BEGIN_DEFINE_SPEC(FEmergeCampsiteSpec, "Emergence.Survival.Campsite",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCampsiteSpec)
void FEmergeCampsiteSpec::Define()
{
	Describe("SafetyScore & IsSafe", [this]()
	{
		It("shelter/water raise, threat lowers, clamped", [this]()
		{
			TestEqual(TEXT("open, no water, no threat -> 0.5"), UEmergeCampsite::SafetyScore(false, false, 0), 0.5f);
			TestEqual(TEXT("sheltered -> 0.8"), UEmergeCampsite::SafetyScore(true, false, 0), 0.8f);
			TestEqual(TEXT("sheltered + water -> 1.0"), UEmergeCampsite::SafetyScore(true, true, 0), 1.0f);
			TestEqual(TEXT("ideal but threat 2 -> 0.5"), UEmergeCampsite::SafetyScore(true, true, 2), 0.5f);
			TestEqual(TEXT("exposed + threat clamps 0"), UEmergeCampsite::SafetyScore(false, false, 2), 0.0f);
			TestTrue(TEXT("0.8 is safe"), UEmergeCampsite::IsSafe(0.8f));
			TestFalse(TEXT("0.5 not safe"), UEmergeCampsite::IsSafe(0.5f));
		});
	});
}
#endif
