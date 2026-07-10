// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: L4D director pacing state machine
// (research: Booth — BuildUp -> SustainPeak 4s -> PeakFade -> Relax 35s or 15000uu -> BuildUp).
// CONTRACT — NEW class UEmergeDirectorState : public UBlueprintFunctionLibrary
//   in Public/AI/EmergeDirectorState.h + Private/AI/EmergeDirectorState.cpp, static UFUNCTION:
//   static int32 Step(int32 State, float Intensity, float TimeInState, float DistTraveledUu);
//     // States: 0=BuildUp 1=SustainPeak 2=PeakFade 3=Relax.
//     // 0: Intensity >= 0.8f -> 1, else 0
//     // 1: TimeInState >= 4.f -> 2, else 1
//     // 2: Intensity <= 0.3f -> 3, else 2
//     // 3: (TimeInState >= 35.f || DistTraveledUu >= 15000.f) -> 0, else 3
//     // State outside 0..3 -> 0
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "AI/EmergeDirectorState.h"

BEGIN_DEFINE_SPEC(FEmergeDirectorStateSpec, "Emergence.AI.DirectorState",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDirectorStateSpec)
void FEmergeDirectorStateSpec::Define()
{
	Describe("Step transitions", [this]()
	{
		It("BuildUp advances at intensity 0.8", [this]()
		{
			TestEqual(TEXT("0 @0.79 -> 0"), UEmergeDirectorState::Step(0, 0.79f, 100.f, 0.f), 0);
			TestEqual(TEXT("0 @0.8 -> 1"), UEmergeDirectorState::Step(0, 0.8f, 0.f, 0.f), 1);
		});
		It("SustainPeak lasts 4 seconds", [this]()
		{
			TestEqual(TEXT("1 @3.9s -> 1"), UEmergeDirectorState::Step(1, 1.f, 3.9f, 0.f), 1);
			TestEqual(TEXT("1 @4s -> 2"), UEmergeDirectorState::Step(1, 1.f, 4.f, 0.f), 2);
		});
		It("PeakFade waits for intensity 0.3", [this]()
		{
			TestEqual(TEXT("2 @0.31 -> 2"), UEmergeDirectorState::Step(2, 0.31f, 60.f, 0.f), 2);
			TestEqual(TEXT("2 @0.3 -> 3"), UEmergeDirectorState::Step(2, 0.3f, 0.f, 0.f), 3);
		});
		It("Relax ends on time OR distance", [this]()
		{
			TestEqual(TEXT("3 @34.9s -> 3"), UEmergeDirectorState::Step(3, 0.f, 34.9f, 0.f), 3);
			TestEqual(TEXT("3 @35s -> 0"), UEmergeDirectorState::Step(3, 0.f, 35.f, 0.f), 0);
			TestEqual(TEXT("3 @1s 15000uu -> 0"), UEmergeDirectorState::Step(3, 0.f, 1.f, 15000.f), 0);
		});
		It("invalid state resets", [this]()
		{
			TestEqual(TEXT("7 -> 0"), UEmergeDirectorState::Step(7, 0.f, 0.f, 0.f), 0);
		});
	});
}
#endif
