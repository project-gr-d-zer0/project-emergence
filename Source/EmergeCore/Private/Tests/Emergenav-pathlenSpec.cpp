// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: measure remaining path length along a
// waypoint polyline (GetNavProgress reports this; straight-line distRemaining under-reports around corners).
// CONTRACT — NEW class UEmergeNavPathLen : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeNavPathLen.h + Private/Nav/EmergeNavPathLen.cpp, static UFUNCTIONs:
//   static float RemainingLength(const TArray<FVector>& Points, int32 FromIdx, FVector Current);
//     // if (Points.Num() == 0 || FromIdx >= Points.Num()) return 0.0f;
//     // int32 i = FMath::Max(0, FromIdx);
//     // float Sum = FVector::Dist(Current, Points[i]);
//     // for (; i + 1 < Points.Num(); ++i) Sum += FVector::Dist(Points[i], Points[i+1]);
//     // return Sum;
//   static float ProgressFraction(float TotalLength, float RemainingLength);
//     // if (TotalLength <= 0.0f) return 1.0f;
//     // return FMath::Clamp(1.0f - RemainingLength / TotalLength, 0.0f, 1.0f);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeNavPathLen.h"

BEGIN_DEFINE_SPEC(FEmergeNavPathLenSpec, "Emergence.Nav.PathLen",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNavPathLenSpec)
void FEmergeNavPathLenSpec::Define()
{
	Describe("RemainingLength", [this]()
	{
		It("sums current->wp then wp->wp", [this]()
		{
			TArray<FVector> P; P.Add(FVector(100,0,0)); P.Add(FVector(100,100,0)); P.Add(FVector(200,100,0));
			// current at (0,0,0), fromIdx 0: 100 + 100 + 100 = 300
			TestEqual(TEXT("L-path -> 300"), UEmergeNavPathLen::RemainingLength(P, 0, FVector(0,0,0)), 300.f);
			TestEqual(TEXT("empty -> 0"), UEmergeNavPathLen::RemainingLength(TArray<FVector>(), 0, FVector(0,0,0)), 0.f);
		});
	});
	Describe("ProgressFraction", [this]()
	{
		It("fraction traveled, clamped", [this]()
		{
			TestEqual(TEXT("half"), UEmergeNavPathLen::ProgressFraction(300.f, 150.f), 0.5f);
			TestEqual(TEXT("done"), UEmergeNavPathLen::ProgressFraction(300.f, 0.f), 1.f);
			TestEqual(TEXT("zero total -> 1"), UEmergeNavPathLen::ProgressFraction(0.f, 0.f), 1.f);
		});
	});
}
#endif
