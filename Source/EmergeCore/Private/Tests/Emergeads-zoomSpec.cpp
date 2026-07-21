// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ADS FOV zoom (combat/gunplay).
// Aiming through an optic narrows the FOV by the scope's magnification and slows look sensitivity to match, so a
// 4x scope both zooms in and steadies the aim. Standard optics math; deterministic.
// CONTRACT — NEW class UEmergeAdsZoom : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeAdsZoom.h + Private/Combat/EmergeAdsZoom.cpp, static UFUNCTIONs.
//
//   static float ZoomedFov(float BaseFov, float Magnification);          // return BaseFov / Magnification;
//   static float SensitivityMultiplier(float Magnification);             // return 1.0f / Magnification;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeAdsZoom.h"

BEGIN_DEFINE_SPEC(FEmergeAdsZoomSpec, "Emergence.Combat.AdsZoom",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAdsZoomSpec)
void FEmergeAdsZoomSpec::Define()
{
	Describe("ZoomedFov & sensitivity", [this]()
	{
		It("magnification narrows FOV and slows sensitivity", [this]()
		{
			TestEqual(TEXT("1x -> base fov"), UEmergeAdsZoom::ZoomedFov(90.f, 1.f), 90.0f);
			TestEqual(TEXT("2x -> half fov"), UEmergeAdsZoom::ZoomedFov(90.f, 2.f), 45.0f);
			TestEqual(TEXT("4x -> quarter fov"), UEmergeAdsZoom::ZoomedFov(90.f, 4.f), 22.5f);
			TestEqual(TEXT("1x sensitivity 1.0"), UEmergeAdsZoom::SensitivityMultiplier(1.f), 1.0f);
			TestEqual(TEXT("2x sensitivity 0.5"), UEmergeAdsZoom::SensitivityMultiplier(2.f), 0.5f);
			TestEqual(TEXT("4x sensitivity 0.25"), UEmergeAdsZoom::SensitivityMultiplier(4.f), 0.25f);
		});
	});
}
#endif
