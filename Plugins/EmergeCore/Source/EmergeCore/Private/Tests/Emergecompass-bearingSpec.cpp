// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: compass bearing marker (shell).
// SHELL stratum: places a waypoint marker on the horizontal compass strip. Computes the signed bearing of the
// target relative to where the player faces ([-180,180]), maps it to a normalized strip offset within the view
// FOV ([-1,1]), and culls markers outside the FOV. Standard bearing math; deterministic.
// CONTRACT — NEW class UEmergeCompassBearing : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeCompassBearing.h + Private/Shell/EmergeCompassBearing.cpp, static UFUNCTIONs.
//
//   static float RelativeBearingDeg(float TargetBearing, float FacingBearing);
//     // signed shortest angle in [-180,180]:
//     // float d = FMath::Fmod(TargetBearing - FacingBearing + 180.0f, 360.0f);  if (d < 0.0f) d += 360.0f;  return d - 180.0f;
//   static float MarkerOffset(float RelativeBearingDeg, float FovDeg);
//     // return FMath::Clamp(RelativeBearingDeg / (FovDeg / 2.0f), -1.0f, 1.0f);
//   static bool  IsInView(float RelativeBearingDeg, float FovDeg);
//     // return FMath::Abs(RelativeBearingDeg) <= (FovDeg / 2.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeCompassBearing.h"

BEGIN_DEFINE_SPEC(FEmergeCompassBearingSpec, "Emergence.Shell.CompassBearing",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCompassBearingSpec)
void FEmergeCompassBearingSpec::Define()
{
	Describe("RelativeBearingDeg", [this]()
	{
		It("signed shortest angle, wrapping", [this]()
		{
			TestEqual(TEXT("target right"), UEmergeCompassBearing::RelativeBearingDeg(90.f, 0.f), 90.0f);
			TestEqual(TEXT("target left"), UEmergeCompassBearing::RelativeBearingDeg(0.f, 90.f), -90.0f);
			TestEqual(TEXT("350 vs 0 -> -10"), UEmergeCompassBearing::RelativeBearingDeg(350.f, 0.f), -10.0f);
			TestEqual(TEXT("10 vs 350 -> +20"), UEmergeCompassBearing::RelativeBearingDeg(10.f, 350.f), 20.0f);
		});
	});

	Describe("MarkerOffset & IsInView", [this]()
	{
		It("maps bearing to strip offset within FOV and culls", [this]()
		{
			TestEqual(TEXT("dead ahead -> 0"), UEmergeCompassBearing::MarkerOffset(0.f, 90.f), 0.0f);
			TestEqual(TEXT("edge of FOV -> 1"), UEmergeCompassBearing::MarkerOffset(45.f, 90.f), 1.0f);
			TestEqual(TEXT("quarter -> 0.5"), UEmergeCompassBearing::MarkerOffset(22.5f, 90.f), 0.5f);
			TestEqual(TEXT("beyond FOV clamps"), UEmergeCompassBearing::MarkerOffset(90.f, 90.f), 1.0f);
			TestTrue(TEXT("45 within 90 FOV"), UEmergeCompassBearing::IsInView(45.f, 90.f));
			TestFalse(TEXT("46 outside"), UEmergeCompassBearing::IsInView(46.f, 90.f));
		});
	});
}
#endif
