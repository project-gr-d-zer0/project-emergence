// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: curved detection ramp.
// Slows the "identify" ramp non-linearly with distance so close-range detection stays quick but
// mid/long-range detection is much slower (stealth feel). Drives AEmergeEnemyController awareness.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeAwareness (do NOT rename or
//   duplicate the class; keep FillRate/Accumulate/Decay/StateFor exactly as they are), in the existing
//   Public/Nav/EmergeAwareness.h + Private/Nav/EmergeAwareness.cpp:
//   static float FillRateCurved(float Dist, float MaxDist, float Exponent, bool bHasLineOfSight);
//     // if (!bHasLineOfSight || MaxDist <= 0.0f || Dist >= MaxDist) { return 0.0f; }
//     // const float Proximity = 1.0f - FMath::Clamp(Dist / MaxDist, 0.0f, 1.0f);
//     // return FMath::Pow(Proximity, FMath::Max(Exponent, 0.1f));
// Deterministic, exact golden values, boundaries covered. Every function body wrapped in braces;
// every code path must return a value.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeAwareness.h"

BEGIN_DEFINE_SPEC(FEmergeAwarenessCurvedSpec, "Emergence.Nav.AwarenessCurved",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAwarenessCurvedSpec)
void FEmergeAwarenessCurvedSpec::Define()
{
	Describe("FillRateCurved (proximity^exponent ramp)", [this]()
	{
		It("matches exact golden values at exponent 2", [this]()
		{
			TestEqual(TEXT("point blank -> 1"), UEmergeAwareness::FillRateCurved(0.f, 1000.f, 2.f, true), 1.f);
			TestEqual(TEXT("half range -> 0.25"), UEmergeAwareness::FillRateCurved(500.f, 1000.f, 2.f, true), 0.25f);
			TestEqual(TEXT("3/4 range -> 0.0625"), UEmergeAwareness::FillRateCurved(750.f, 1000.f, 2.f, true), 0.0625f);
			TestEqual(TEXT("at max range -> 0"), UEmergeAwareness::FillRateCurved(1000.f, 1000.f, 2.f, true), 0.f);
			TestEqual(TEXT("beyond max -> 0"), UEmergeAwareness::FillRateCurved(1500.f, 1000.f, 2.f, true), 0.f);
		});
		It("reduces to the linear ramp at exponent 1", [this]()
		{
			TestEqual(TEXT("half range exp1 -> 0.5"), UEmergeAwareness::FillRateCurved(500.f, 1000.f, 1.f, true), 0.5f);
			TestEqual(TEXT("quarter range exp1 -> 0.75"), UEmergeAwareness::FillRateCurved(250.f, 1000.f, 1.f, true), 0.75f);
		});
		It("gates and clamps boundaries", [this]()
		{
			TestEqual(TEXT("no LOS -> 0"), UEmergeAwareness::FillRateCurved(100.f, 1000.f, 2.f, false), 0.f);
			TestEqual(TEXT("zero max dist -> 0"), UEmergeAwareness::FillRateCurved(100.f, 0.f, 2.f, true), 0.f);
			TestEqual(TEXT("negative dist clamps -> 1"), UEmergeAwareness::FillRateCurved(-100.f, 1000.f, 2.f, true), 1.f);
			TestEqual(TEXT("exponent floors at 0.1"), UEmergeAwareness::FillRateCurved(500.f, 1000.f, 0.f, true), 0.9330329915f, 1.e-4f);
		});
	});
}
#endif

