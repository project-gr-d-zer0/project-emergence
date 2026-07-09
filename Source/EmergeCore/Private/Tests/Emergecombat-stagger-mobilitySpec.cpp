// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: stagger -> mobility tier.
// Deterministic mapping the ALS locomotion bridge reads every frame: 0 = ragdoll (knockdown/dead),
// 1 = forced walk (stumble/stagger/crawl), 2 = run, 3 = sprint (only when wanted AND stamina allows).
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeStagger (do NOT rename/duplicate it;
//   keep every existing method exactly), in the existing
//   Public/Combat/EmergeStagger.h + Private/Combat/EmergeStagger.cpp:
//   static int32 MobilityForState(EEmergeStaggerState State, bool bWantsSprint, bool bCanSprint);
//     // switch (State) {
//     // case EEmergeStaggerState::Knockdown:
//     // case EEmergeStaggerState::Dead: return 0;
//     // case EEmergeStaggerState::Stumble:
//     // case EEmergeStaggerState::Stagger:
//     // case EEmergeStaggerState::Crawl: return 1;
//     // default: return (bWantsSprint && bCanSprint) ? 3 : 2; }
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeStagger.h"

BEGIN_DEFINE_SPEC(FEmergeStaggerMobilitySpec, "Emergence.Combat.StaggerMobility",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaggerMobilitySpec)
void FEmergeStaggerMobilitySpec::Define()
{
	Describe("MobilityForState (0 ragdoll / 1 walk / 2 run / 3 sprint)", [this]()
	{
		It("maps every stagger state and gates sprint on stamina", [this]()
		{
			TestEqual(TEXT("knockdown -> ragdoll 0"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Knockdown, true, true), 0);
			TestEqual(TEXT("dead -> ragdoll 0"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Dead, false, false), 0);
			TestEqual(TEXT("stumble -> walk 1"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Stumble, true, true), 1);
			TestEqual(TEXT("stagger -> walk 1"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Stagger, false, true), 1);
			TestEqual(TEXT("crawl -> walk 1"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Crawl, true, true), 1);
			TestEqual(TEXT("moving + sprint + stamina -> 3"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Moving, true, true), 3);
			TestEqual(TEXT("moving + sprint + NO stamina -> 2"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Moving, true, false), 2);
			TestEqual(TEXT("moving no sprint -> 2"), UEmergeStagger::MobilityForState(EEmergeStaggerState::Moving, false, true), 2);
		});
	});
}
#endif
