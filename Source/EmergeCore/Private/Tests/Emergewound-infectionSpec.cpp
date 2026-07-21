// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: wound infection stages (DayZ-style).
// A dirty wound (WOUND_AGENT, seeded by EmergeBandage's dirty-dressing path) escalates in stages: none, early
// infection, then severe infection with fever and health damage over time. Closes the wound->dirty-bandage->
// infection loop. Numbers exact from decompiled DayZ woundinfection.c / woundinfection2.c: stage-1 activates at
// WOUND_AGENT 100, stage-2 (fever) at 250, stage-2 DAMAGE_PER_SEC 0.04.
// CONTRACT — NEW class UEmergeWoundInfection : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeWoundInfection.h + Private/Survival/EmergeWoundInfection.cpp, static UFUNCTIONs.
//
//   static int32 InfectionStage(int32 WoundAgentCount);
//     // if (WoundAgentCount >= 250) return 2;  if (WoundAgentCount >= 100) return 1;  return 0;
//   static bool  HasFever(int32 WoundAgentCount);   // return WoundAgentCount >= 250;  // stage 2
//   static float StageTwoDamagePerTick(float DeltaT);   // return 0.04f * DeltaT;   // WoundInfection2 DAMAGE_PER_SEC
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeWoundInfection.h"

BEGIN_DEFINE_SPEC(FEmergeWoundInfectionSpec, "Emergence.Survival.WoundInfection",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWoundInfectionSpec)
void FEmergeWoundInfectionSpec::Define()
{
	Describe("InfectionStage", [this]()
	{
		It("escalates at agent 100 and 250", [this]()
		{
			TestEqual(TEXT("50 -> none"), UEmergeWoundInfection::InfectionStage(50), 0);
			TestEqual(TEXT("99 -> none"), UEmergeWoundInfection::InfectionStage(99), 0);
			TestEqual(TEXT("100 -> stage 1"), UEmergeWoundInfection::InfectionStage(100), 1);
			TestEqual(TEXT("200 -> stage 1"), UEmergeWoundInfection::InfectionStage(200), 1);
			TestEqual(TEXT("250 -> stage 2"), UEmergeWoundInfection::InfectionStage(250), 2);
			TestEqual(TEXT("400 -> stage 2"), UEmergeWoundInfection::InfectionStage(400), 2);
		});
	});

	Describe("Fever & stage-2 damage", [this]()
	{
		It("fever and health drain begin at stage 2", [this]()
		{
			TestTrue(TEXT("250 fever"), UEmergeWoundInfection::HasFever(250));
			TestFalse(TEXT("249 no fever"), UEmergeWoundInfection::HasFever(249));
			TestEqual(TEXT("damage 0.04/s"), UEmergeWoundInfection::StageTwoDamagePerTick(1.0f), 0.04f);
			TestEqual(TEXT("damage dt=2 -> 0.08"), UEmergeWoundInfection::StageTwoDamagePerTick(2.0f), 0.08f);
		});
	});
}
#endif
