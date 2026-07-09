// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: world-event math (research #10).
// The persistent world's raid-timer substitute: (a) horde SURGE temporarily shifts effective depth
// (reusing the depth-difficulty kernels); (b) shelter check gates surge lethality; (c) the more loot
// value a player carries unsecured, the more map-visible ("hunted") they are - Hunt-style broadcast.
// CONTRACT — NEW class UEmergeWorldEvents : public UBlueprintFunctionLibrary
//   in Public/World/EmergeWorldEvents.h + Private/World/EmergeWorldEvents.cpp, static UFUNCTIONs:
//   static int32 SurgeDepthShift(int32 BaseDepth, bool bSurgeActive);
//     // return bSurgeActive ? FMath::Max(0, BaseDepth) + 2 : FMath::Max(0, BaseDepth);
//   static bool IsSheltered(bool bIndoors, bool bUnderground);
//     // return bIndoors || bUnderground;
//   static float BroadcastVisibilityWeight(int32 ValueAtRisk);
//     // return FMath::Clamp((ValueAtRisk - 500) / 4500.0f, 0.0f, 1.0f);  // invisible under 500, max at 5000
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "World/EmergeWorldEvents.h"

BEGIN_DEFINE_SPEC(FEmergeWorldEventsSpec, "Emergence.World.Events",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWorldEventsSpec)
void FEmergeWorldEventsSpec::Define()
{
	Describe("SurgeDepthShift (surge = the world gets 2 depths meaner)", [this]()
	{
		It("shifts only while active and floors negatives", [this]()
		{
			TestEqual(TEXT("depth 1 surge -> 3"), UEmergeWorldEvents::SurgeDepthShift(1, true), 3);
			TestEqual(TEXT("depth 1 calm -> 1"), UEmergeWorldEvents::SurgeDepthShift(1, false), 1);
			TestEqual(TEXT("negative depth surge -> 2"), UEmergeWorldEvents::SurgeDepthShift(-4, true), 2);
		});
	});

	Describe("IsSheltered (indoors or underground)", [this]()
	{
		It("either qualifies; open air does not", [this]()
		{
			TestTrue(TEXT("indoors"), UEmergeWorldEvents::IsSheltered(true, false));
			TestTrue(TEXT("underground"), UEmergeWorldEvents::IsSheltered(false, true));
			TestFalse(TEXT("open air"), UEmergeWorldEvents::IsSheltered(false, false));
		});
	});

	Describe("BroadcastVisibilityWeight (carrying more = more hunted)", [this]()
	{
		It("matches exact golden values and clamps", [this]()
		{
			TestEqual(TEXT("empty pockets 0 -> 0"), UEmergeWorldEvents::BroadcastVisibilityWeight(0), 0.f);
			TestEqual(TEXT("500 threshold -> 0"), UEmergeWorldEvents::BroadcastVisibilityWeight(500), 0.f);
			TestEqual(TEXT("2750 -> 0.5"), UEmergeWorldEvents::BroadcastVisibilityWeight(2750), 0.5f);
			TestEqual(TEXT("5000 -> 1"), UEmergeWorldEvents::BroadcastVisibilityWeight(5000), 1.f);
			TestEqual(TEXT("9999 clamps -> 1"), UEmergeWorldEvents::BroadcastVisibilityWeight(9999), 1.f);
		});
	});
}
#endif
