// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: deterministic path-following math for
// the NavigateTo harness (arrival test, waypoint advance, progress fraction).
// CONTRACT — NEW class UEmergeNavFollow : public UBlueprintFunctionLibrary
//   in Public/Sensor/EmergeNavFollow.h + Private/Sensor/EmergeNavFollow.cpp, static UFUNCTIONs:
//   static bool ArrivedAt(float DistToGoal, float AcceptRadius);       // return DistToGoal <= AcceptRadius;
//   static bool ShouldAdvanceWaypoint(float DistToWaypoint, float AdvanceRadius);  // return DistToWaypoint <= AdvanceRadius;
//   static float NavProgress(float TotalPathLen, float RemainingLen);
//     // return (TotalPathLen <= 0.0f) ? 1.0f : FMath::Clamp(1.0f - RemainingLen / TotalPathLen, 0.0f, 1.0f);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeNavFollow.h"

BEGIN_DEFINE_SPEC(FEmergeNavFollowSpec, "Emergence.Nav.Follow",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNavFollowSpec)
void FEmergeNavFollowSpec::Define()
{
	Describe("ArrivedAt / ShouldAdvanceWaypoint", [this]()
	{
		It("within radius", [this]()
		{
			TestTrue(TEXT("50<=100 arrived"), UEmergeNavFollow::ArrivedAt(50.f, 100.f));
			TestFalse(TEXT("150>100 not arrived"), UEmergeNavFollow::ArrivedAt(150.f, 100.f));
			TestTrue(TEXT("80<=100 advance"), UEmergeNavFollow::ShouldAdvanceWaypoint(80.f, 100.f));
		});
	});
	Describe("NavProgress", [this]()
	{
		It("fraction complete, guards", [this]()
		{
			TestEqual(TEXT("200 total 50 left -> 0.75"), UEmergeNavFollow::NavProgress(200.f, 50.f), 0.75f);
			TestEqual(TEXT("nothing done -> 0"), UEmergeNavFollow::NavProgress(200.f, 200.f), 0.f);
			TestEqual(TEXT("zero total -> 1"), UEmergeNavFollow::NavProgress(0.f, 0.f), 1.f);
		});
	});
}
#endif
