// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: objective tracker view-model (shell).
// SHELL stratum: the quest/objective readout - progress fraction toward a target, remaining count, and completion.
// Pure view math; deterministic.
// CONTRACT — NEW class UEmergeObjectiveTracker : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeObjectiveTracker.h + Private/Shell/EmergeObjectiveTracker.cpp, static UFUNCTIONs.
//
//   static float Progress01(int32 Current, int32 Target);   // return FMath::Clamp((float)Current / (float)Target, 0.f, 1.f);
//   static bool  IsComplete(int32 Current, int32 Target);    // return Current >= Target;
//   static int32 Remaining(int32 Current, int32 Target);     // return FMath::Max(0, Target - Current);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeObjectiveTracker.h"

BEGIN_DEFINE_SPEC(FEmergeObjectiveTrackerSpec, "Emergence.Shell.ObjectiveTracker",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeObjectiveTrackerSpec)
void FEmergeObjectiveTrackerSpec::Define()
{
	Describe("Progress, completion, remaining", [this]()
	{
		It("tracks toward the target and clamps", [this]()
		{
			TestEqual(TEXT("3/10 -> 0.3"), UEmergeObjectiveTracker::Progress01(3, 10), 0.3f);
			TestEqual(TEXT("10/10 -> 1.0"), UEmergeObjectiveTracker::Progress01(10, 10), 1.0f);
			TestEqual(TEXT("overshoot clamps 1"), UEmergeObjectiveTracker::Progress01(15, 10), 1.0f);
			TestFalse(TEXT("9/10 not complete"), UEmergeObjectiveTracker::IsComplete(9, 10));
			TestTrue(TEXT("10/10 complete"), UEmergeObjectiveTracker::IsComplete(10, 10));
			TestTrue(TEXT("over target complete"), UEmergeObjectiveTracker::IsComplete(12, 10));
			TestEqual(TEXT("3/10 -> 7 remaining"), UEmergeObjectiveTracker::Remaining(3, 10), 7);
			TestEqual(TEXT("done -> 0 remaining"), UEmergeObjectiveTracker::Remaining(12, 10), 0);
		});
	});
}
#endif
