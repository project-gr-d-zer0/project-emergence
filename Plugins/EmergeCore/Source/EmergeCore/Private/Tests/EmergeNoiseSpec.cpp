// HUMAN-AUTHORED trusted acceptance spec. Feature: noise emission/propagation/detection kernel.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "World/EmergeNoise.h"

BEGIN_DEFINE_SPEC(FEmergeNoiseSpec, "Emergence.World.Noise",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNoiseSpec)
void FEmergeNoiseSpec::Define()
{
	Describe("EffectiveRadius (env x depth modified)", [this]()
	{
		It("matches exact golden values and floors at 0", [this]()
		{
			TestEqual(TEXT("200 * 0.75 * 1.5 -> 225"), UEmergeNoise::EffectiveRadius(200.f, 0.75f, 1.5f), 225.f);
			TestEqual(TEXT("unmodified passthrough"),   UEmergeNoise::EffectiveRadius(100.f, 1.f, 1.f), 100.f);
			TestEqual(TEXT("negative env floors at 0"), UEmergeNoise::EffectiveRadius(100.f, -1.f, 1.f), 0.f);
		});
	});

	Describe("IsHeard (within radius)", [this]()
	{
		It("hears at/inside the boundary, not beyond, guards negative distance", [this]()
		{
			TestTrue(TEXT("inside 100@50"), UEmergeNoise::IsHeard(100.f, 50.f));
			TestTrue(TEXT("boundary 100@100"), UEmergeNoise::IsHeard(100.f, 100.f));
			TestFalse(TEXT("beyond 100@101"), UEmergeNoise::IsHeard(100.f, 101.f));
			TestFalse(TEXT("negative distance"), UEmergeNoise::IsHeard(100.f, -1.f));
		});
	});

	Describe("AttractionWeight (volume/distance)", [this]()
	{
		It("matches exact golden values and guards zero distance", [this]()
		{
			TestEqual(TEXT("100 vol @ 50m -> 2"), UEmergeNoise::AttractionWeight(100.f, 50.f), 2.f);
			TestEqual(TEXT("point-blank uses 1m floor -> 100"), UEmergeNoise::AttractionWeight(100.f, 0.f), 100.f);
			TestEqual(TEXT("negative volume -> 0"), UEmergeNoise::AttractionWeight(-5.f, 10.f), 0.f);
		});
	});

	Describe("ResponseToShot (suppressor asymmetry)", [this]()
	{
		It("unsuppressed = exact aggro; suppressed = LOS-gated; unheard = ignore", [this]()
		{
			TestEqual(TEXT("not heard -> Ignore"),
				(int32)UEmergeNoise::ResponseToShot(false, false, true), (int32)EEmergeNoiseResponse::Ignore);
			TestEqual(TEXT("unsuppressed heard -> TargetExact even w/o LOS"),
				(int32)UEmergeNoise::ResponseToShot(false, true, false), (int32)EEmergeNoiseResponse::TargetExact);
			TestEqual(TEXT("suppressed heard + LOS -> TargetExact"),
				(int32)UEmergeNoise::ResponseToShot(true, true, true), (int32)EEmergeNoiseResponse::TargetExact);
			TestEqual(TEXT("suppressed heard no LOS -> Investigate"),
				(int32)UEmergeNoise::ResponseToShot(true, true, false), (int32)EEmergeNoiseResponse::Investigate);
		});
	});
}
#endif
