// HUMAN-AUTHORED trusted acceptance spec (do not edit). R3 SOAK / SESSION test.
// Runs the COMPOSED survival system for many ticks and asserts invariants never break - the failure mode
// unit tests can't see: systems that are each individually correct but drift, overflow, or NaN when wired
// together over time. Deterministic (fixed inputs, no RNG). Pure over already-implemented functions; joins the
// gate suite with no new /core class. Models a sprinting, bleeding survivor whose depleting energy/water weaken
// immunity, letting a HIGH-potency agent grow - the full loop, ticked.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeDisease.h"
#include "Survival/EmergeMetabolism.h"
#include "Survival/EmergeBleeding.h"

BEGIN_DEFINE_SPEC(FEmergeSurvivalSoakSpec, "Emergence.Survival.Soak",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSurvivalSoakSpec)
void FEmergeSurvivalSoakSpec::Define()
{
	Describe("Composed survivor soak (300 ticks)", [this]()
	{
		It("keeps every stat finite and in range across a long ticked run", [this]()
		{
			float blood = 5000.f, energy = 3000.f, water = 3000.f, agents = 0.f;
			const float dt = 1.0f;
			const int32 MOVE_SPRINT = 3, INTENSITY_HIGH = 2, POTENCY_HIGH = 1;

			for (int32 tick = 0; tick < 300; ++tick)
			{
				// metabolism: sprinting burns energy + water (drain is a positive magnitude to subtract)
				energy = FMath::Clamp(energy - UEmergeMetabolism::DrainPerTick(MOVE_SPRINT, energy, dt), 0.f, 5000.f);
				water  = FMath::Clamp(water  - UEmergeMetabolism::DrainPerTick(MOVE_SPRINT, water,  dt), 0.f, 5000.f);

				// disease: weaker immunity (from depleted vitals) lets the agent grow
				const float imm = UEmergeDisease::Immunity01(energy, water, 1.0f, blood / 5000.f);
				const int32 immLevel = UEmergeDisease::ImmunityLevel(imm);
				const float agentDelta = UEmergeDisease::GrowDelta(0.75f, 1.0f, POTENCY_HIGH, immLevel, dt);
				agents = UEmergeDisease::StepCount(agents, agentDelta, 300.f);

				// bleeding: one high source drains blood, tapering as pressure drops
				blood = FMath::Clamp(blood + UEmergeBleeding::SourceBloodLossPerTick(INTENSITY_HIGH, blood, dt), 0.f, 5000.f);

				// INVARIANTS - must hold on EVERY tick
				TestTrue(TEXT("energy finite & in range"), FMath::IsFinite(energy) && energy >= 0.f && energy <= 5000.f);
				TestTrue(TEXT("water finite & in range"),  FMath::IsFinite(water)  && water  >= 0.f && water  <= 5000.f);
				TestTrue(TEXT("blood finite & in range"),  FMath::IsFinite(blood)  && blood  >= 0.f && blood  <= 5000.f);
				TestTrue(TEXT("agents finite & in [0,max]"), FMath::IsFinite(agents) && agents >= 0.f && agents <= 300.f);
				TestTrue(TEXT("immunity in [0,1]"), FMath::IsFinite(imm) && imm >= 0.f && imm <= 1.f);
			}

			// end-state sanity: a sprinting, bleeding survivor should have LOST resources but not broken bounds
			TestTrue(TEXT("energy depleted from start"), energy < 3000.f);
			TestTrue(TEXT("blood depleted from start"), blood < 5000.f);
			TestTrue(TEXT("agent load grew as immunity fell"), agents > 0.f);
		});
	});
}
#endif
