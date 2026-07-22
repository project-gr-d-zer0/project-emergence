// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: activated charcoal detox (DayZ-style).
// Charcoal tablets actively purge food-poison agents (EmergeDisease/EmergePoisoning) - without them the agent
// only dies off slowly, with them it clears at a fixed rate. Numbers exact from decompiled DayZ foodpoisonagent.c
// GetDieOffSpeedEx: 0.5/s while the CHARCOAL modifier is active.
// CONTRACT — NEW class UEmergeCharcoal : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeCharcoal.h + Private/Survival/EmergeCharcoal.cpp, static UFUNCTIONs.
//
//   static float FoodPoisonDieOffPerSec(bool bCharcoalActive);   // return bCharcoalActive ? 0.5f : 0.0f;
//   static float AgentReducedPerTick(bool bCharcoalActive, float DeltaT);
//     // return FoodPoisonDieOffPerSec(bCharcoalActive) * DeltaT;
//   static float TimeToClear(float AgentCount, bool bCharcoalActive);
//     // return bCharcoalActive ? (AgentCount / 0.5f) : -1.0f;   // -1 = never clears on its own
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeCharcoal.h"

BEGIN_DEFINE_SPEC(FEmergeCharcoalSpec, "Emergence.Survival.Charcoal",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCharcoalSpec)
void FEmergeCharcoalSpec::Define()
{
	Describe("Charcoal detox", [this]()
	{
		It("clears food-poison agents at 0.5/s while active", [this]()
		{
			TestEqual(TEXT("active dieoff 0.5/s"), UEmergeCharcoal::FoodPoisonDieOffPerSec(true), 0.5f);
			TestEqual(TEXT("inactive 0"), UEmergeCharcoal::FoodPoisonDieOffPerSec(false), 0.0f);
			TestEqual(TEXT("active dt=1 -> 0.5"), UEmergeCharcoal::AgentReducedPerTick(true, 1.0f), 0.5f);
			TestEqual(TEXT("active dt=2 -> 1.0"), UEmergeCharcoal::AgentReducedPerTick(true, 2.0f), 1.0f);
			TestEqual(TEXT("inactive -> 0"), UEmergeCharcoal::AgentReducedPerTick(false, 1.0f), 0.0f);
			TestEqual(TEXT("300 agents clear in 600s"), UEmergeCharcoal::TimeToClear(300.f, true), 600.0f);
			TestEqual(TEXT("no charcoal -> never (-1)"), UEmergeCharcoal::TimeToClear(300.f, false), -1.0f);
		});
	});
}
#endif
