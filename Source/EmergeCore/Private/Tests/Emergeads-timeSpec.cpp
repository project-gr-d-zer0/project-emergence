// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ADS (aim-down-sight) time (combat/gunplay).
// Tarkov-style handling: how long it takes to raise the sights, driven by weapon ergonomics (higher = faster)
// and a weight penalty (heavier = slower). TUNABLE design model, fixed + deterministic: at full ergonomics and
// no weight penalty ADS is the base time; poor ergonomics up to doubles it; weight penalty up to doubles it.
// CONTRACT — NEW class UEmergeAdsTime : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeAdsTime.h + Private/Combat/EmergeAdsTime.cpp, static UFUNCTIONs.
//
//   static float AdsTimeSec(float BaseTime, float Ergonomics01, float WeightPenalty01);
//     // const float e = FMath::Clamp(Ergonomics01, 0.0f, 1.0f);
//     // const float w = FMath::Clamp(WeightPenalty01, 0.0f, 1.0f);
//     // return BaseTime * (2.0f - e) * (1.0f + w);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeAdsTime.h"

BEGIN_DEFINE_SPEC(FEmergeAdsTimeSpec, "Emergence.Combat.AdsTime",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAdsTimeSpec)
void FEmergeAdsTimeSpec::Define()
{
	Describe("AdsTimeSec", [this]()
	{
		It("fast at high ergonomics, slower with poor ergo and weight", [this]()
		{
			TestEqual(TEXT("full ergo, no weight -> base"), UEmergeAdsTime::AdsTimeSec(0.3f, 1.0f, 0.0f), 0.3f);
			TestEqual(TEXT("zero ergo doubles -> 0.6"), UEmergeAdsTime::AdsTimeSec(0.3f, 0.0f, 0.0f), 0.6f);
			TestEqual(TEXT("half ergo -> 0.45"), UEmergeAdsTime::AdsTimeSec(0.3f, 0.5f, 0.0f), 0.45f);
			TestEqual(TEXT("full weight penalty doubles -> 0.6"), UEmergeAdsTime::AdsTimeSec(0.3f, 1.0f, 1.0f), 0.6f);
			TestEqual(TEXT("half weight -> 0.45"), UEmergeAdsTime::AdsTimeSec(0.3f, 1.0f, 0.5f), 0.45f);
			TestEqual(TEXT("ergo clamps above 1"), UEmergeAdsTime::AdsTimeSec(0.3f, 1.5f, 0.0f), 0.3f);
		});
	});
}
#endif
