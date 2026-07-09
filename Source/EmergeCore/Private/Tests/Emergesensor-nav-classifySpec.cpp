// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: interpret UE nav-query results as
// clean telemetry booleans. ENavigationQueryResult: Invalid=0,Error=1,Fail=2,Success=3.
// CONTRACT — NEW class UEmergeNavClassify : public UBlueprintFunctionLibrary
//   in Public/Sensor/EmergeNavClassify.h + Private/Sensor/EmergeNavClassify.cpp, static UFUNCTIONs:
//   static bool IsReachable(int32 QueryResult, bool bIsPartial);
//     // return QueryResult == 3 && !bIsPartial;      // Success and a complete (non-partial) path
//   static float DetourRatio(float PathLength, float StraightDist);
//     // return (StraightDist <= 0.0f) ? 1.0f : PathLength / StraightDist;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeNavClassify.h"

BEGIN_DEFINE_SPEC(FEmergeNavClassifySpec, "Emergence.Sensor.Nav",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNavClassifySpec)
void FEmergeNavClassifySpec::Define()
{
	Describe("IsReachable (Success && !partial)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestTrue(TEXT("success complete -> true"), UEmergeNavClassify::IsReachable(3, false));
			TestFalse(TEXT("success partial -> false"), UEmergeNavClassify::IsReachable(3, true));
			TestFalse(TEXT("fail -> false"), UEmergeNavClassify::IsReachable(2, false));
		});
	});
	Describe("DetourRatio", [this]()
	{
		It("matches exact golden values and guards zero", [this]()
		{
			TestEqual(TEXT("150/100 -> 1.5"), UEmergeNavClassify::DetourRatio(150.f, 100.f), 1.5f);
			TestEqual(TEXT("straight -> 1"), UEmergeNavClassify::DetourRatio(100.f, 100.f), 1.f);
			TestEqual(TEXT("zero straight -> 1"), UEmergeNavClassify::DetourRatio(50.f, 0.f), 1.f);
		});
	});
}
#endif
