// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: gradual detection ("slow identify").
// The NPC's awareness of a target fills over time (faster when close + line-of-sight), decays when unseen.
// CONTRACT — NEW class UEmergeAwareness : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeAwareness.h + Private/Nav/EmergeAwareness.cpp, static UFUNCTIONs:
//   static float FillRate(float Dist, float MaxDist, bool bHasLineOfSight);
//     // if (!bHasLineOfSight || MaxDist <= 0.0f || Dist >= MaxDist) return 0.0f;   // no build without LOS / out of range
//     // return 1.0f - FMath::Clamp(Dist / MaxDist, 0.0f, 1.0f);                     // closer -> faster fill (per-second)
//   static float Accumulate(float Current, float FillRate, float DtSeconds);
//     // return FMath::Clamp(Current + FillRate * FMath::Max(0.0f, DtSeconds), 0.0f, 1.0f);
//   static float Decay(float Current, float DecayPerSecond, float DtSeconds);
//     // return FMath::Clamp(Current - FMath::Max(0.0f, DecayPerSecond) * FMath::Max(0.0f, DtSeconds), 0.0f, 1.0f);
//   static int32 StateFor(float Awareness);   // 0=Unaware,1=Suspicious,2=Alerted,3=Chasing
//     // if (Awareness < 0.25f) return 0; if (Awareness < 0.60f) return 1; if (Awareness < 1.0f) return 2; return 3;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeAwareness.h"

BEGIN_DEFINE_SPEC(FEmergeAwarenessSpec, "Emergence.Nav.Awareness",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAwarenessSpec)
void FEmergeAwarenessSpec::Define()
{
	Describe("FillRate (needs LOS, faster when close)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("no LOS -> 0"), UEmergeAwareness::FillRate(100.f, 1000.f, false), 0.0f);
			TestEqual(TEXT("point blank -> 1"), UEmergeAwareness::FillRate(0.f, 1000.f, true), 1.0f);
			TestEqual(TEXT("half range -> 0.5"), UEmergeAwareness::FillRate(500.f, 1000.f, true), 0.5f);
			TestEqual(TEXT("at range -> 0"), UEmergeAwareness::FillRate(1000.f, 1000.f, true), 0.0f);
		});
	});
	Describe("Accumulate / Decay (clamped 0..1)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("accumulate 0 + 0.5*1s"), UEmergeAwareness::Accumulate(0.f, 0.5f, 1.f), 0.5f);
			TestEqual(TEXT("accumulate caps at 1"), UEmergeAwareness::Accumulate(0.9f, 0.5f, 1.f), 1.0f);
			TestEqual(TEXT("decay 0.5 - 0.2*1s"), UEmergeAwareness::Decay(0.5f, 0.2f, 1.f), 0.3f);
			TestEqual(TEXT("decay floors at 0"), UEmergeAwareness::Decay(0.1f, 0.5f, 1.f), 0.0f);
		});
	});
	Describe("StateFor thresholds", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("0.1 -> Unaware"), UEmergeAwareness::StateFor(0.1f), 0);
			TestEqual(TEXT("0.4 -> Suspicious"), UEmergeAwareness::StateFor(0.4f), 1);
			TestEqual(TEXT("0.8 -> Alerted"), UEmergeAwareness::StateFor(0.8f), 2);
			TestEqual(TEXT("1.0 -> Chasing"), UEmergeAwareness::StateFor(1.0f), 3);
		});
	});
}
#endif
