// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: concrete noise radii.
// Footsteps scale with movement speed; gunshots are map-events unless suppressed. Feeds zombie aggro.
// CONTRACT — ADD TWO static UFUNCTIONs to the EXISTING class UEmergeNoise (do NOT rename/duplicate it;
//   keep EffectiveRadius/IsHeard/AttractionWeight/ResponseToShot exactly), in the existing
//   Public/World/EmergeNoise.h + Private/World/EmergeNoise.cpp:
//   static float FootstepRadiusM(float SpeedRatio);
//     // return 2.0f + 18.0f * FMath::Clamp(SpeedRatio, 0.0f, 1.0f);   // creep 2m ... sprint 20m
//   static float GunshotRadiusM(bool bSuppressed);
//     // return bSuppressed ? 120.0f : 800.0f;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "World/EmergeNoise.h"

BEGIN_DEFINE_SPEC(FEmergeNoiseRadiiSpec, "Emergence.World.NoiseRadii",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNoiseRadiiSpec)
void FEmergeNoiseRadiiSpec::Define()
{
	Describe("FootstepRadiusM (2m creep to 20m sprint)", [this]()
	{
		It("matches exact golden values and clamps", [this]()
		{
			TestEqual(TEXT("still/creep 0 -> 2m"), UEmergeNoise::FootstepRadiusM(0.f), 2.f);
			TestEqual(TEXT("half speed -> 11m"), UEmergeNoise::FootstepRadiusM(0.5f), 11.f);
			TestEqual(TEXT("sprint 1 -> 20m"), UEmergeNoise::FootstepRadiusM(1.f), 20.f);
			TestEqual(TEXT("over 1 clamps -> 20m"), UEmergeNoise::FootstepRadiusM(2.f), 20.f);
			TestEqual(TEXT("negative clamps -> 2m"), UEmergeNoise::FootstepRadiusM(-1.f), 2.f);
		});
	});

	Describe("GunshotRadiusM (suppressor asymmetry, concrete)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("unsuppressed -> 800m"), UEmergeNoise::GunshotRadiusM(false), 800.f);
			TestEqual(TEXT("suppressed -> 120m"), UEmergeNoise::GunshotRadiusM(true), 120.f);
		});
	});
}
#endif
