// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: disease agents / immune system (DayZ-style).
// The missing survival consequence layer: bad food/water/cold weakens immunity, and transmission agents
// (salmonella, cholera, ...) grow inside the player whenever their potency out-competes the immune level,
// then die off once immunity recovers. Ties together food-spoilage (source), wetness+heat-comfort (immunity
// drain) and blood/shock (outcome). Numbers exact from decompiled DayZ: playerconstants.c immunity thresholds
// (crit 0.15 / low 0.35 / normal 0.72 / high 0.95), SL_ENERGY_MAX = SL_WATER_MAX = 5000, EStatLevels ordering
// (GREAT 0, HIGH 1, MEDIUM 2, LOW 3, CRITICAL 4), and playeragentpool.c GrowAgents() growth/die-off simulation.
// Agent params from salmonellaagent.c / choleraagent.c.
// CONTRACT — NEW class UEmergeDisease : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeDisease.h + Private/Survival/EmergeDisease.cpp, static UFUNCTIONs.
//   Provide these named int32 stat-level constants matching EStatLevels: GREAT=0, HIGH=1, MEDIUM=2, LOW=3, CRITICAL=4.
//
//   static float Immunity01(float Energy, float Water, float Health01, float Blood01);
//     // PlayerBase::GetImmunity(): average of the four normalized vitals, clamped to [0,1].
//     // const float e = Energy / 5000.0f;   // SL_ENERGY_MAX
//     // const float w = Water  / 5000.0f;   // SL_WATER_MAX
//     // float imm = (e + w + Health01 + Blood01) / 4.0f;
//     // return FMath::Clamp(imm, 0.0f, 1.0f);
//
//   static int32 ImmunityLevel(float Immunity01);
//     // PlayerBase::GetImmunityLevel() via GetStatLevel(): returns an EStatLevels ordinal (0..4).
//     // if (Immunity01 <= 0.15f) return 4; // CRITICAL
//     // if (Immunity01 <= 0.35f) return 3; // LOW
//     // if (Immunity01 <= 0.72f) return 2; // MEDIUM
//     // if (Immunity01 <= 0.95f) return 1; // HIGH
//     // return 0;                          // GREAT
//
//   static bool AgentGrows(int32 AgentPotency, int32 ImmunityLevel);
//     // GrowAgents(): agent grows when its potency is at or below the immune level (weaker/equal immunity).
//     // return AgentPotency <= ImmunityLevel;
//
//   static float GrowDelta(float Invasibility, float DieOffSpeed, int32 AgentPotency, int32 ImmunityLevel, float DeltaT);
//     // if (AgentGrows(AgentPotency, ImmunityLevel)) return Invasibility * DeltaT;
//     // return -DieOffSpeed * DeltaT;
//
//   static float StepCount(float OldCount, float GrowDelta, float MaxCount);
//     // return FMath::Clamp(OldCount + GrowDelta, 0.0f, MaxCount);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeDisease.h"

BEGIN_DEFINE_SPEC(FEmergeDiseaseAgentSpec, "Emergence.Survival.Disease",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDiseaseAgentSpec)
void FEmergeDiseaseAgentSpec::Define()
{
	Describe("Immunity01", [this]()
	{
		It("averages the four normalized vitals and clamps to [0,1]", [this]()
		{
			TestEqual(TEXT("all full -> 1.0"), UEmergeDisease::Immunity01(5000.f, 5000.f, 1.f, 1.f), 1.0f);
			TestEqual(TEXT("all half -> 0.5"), UEmergeDisease::Immunity01(2500.f, 2500.f, 0.5f, 0.5f), 0.5f);
			TestEqual(TEXT("all empty -> 0.0"), UEmergeDisease::Immunity01(0.f, 0.f, 0.f, 0.f), 0.0f);
			TestEqual(TEXT("mixed -> 0.5"), UEmergeDisease::Immunity01(5000.f, 0.f, 1.f, 0.f), 0.5f);
		});
	});

	Describe("ImmunityLevel", [this]()
	{
		It("maps immunity to the EStatLevels ordinal at exact thresholds", [this]()
		{
			TestEqual(TEXT("0.10 CRITICAL"), UEmergeDisease::ImmunityLevel(0.10f), 4);
			TestEqual(TEXT("0.15 CRITICAL (boundary)"), UEmergeDisease::ImmunityLevel(0.15f), 4);
			TestEqual(TEXT("0.30 LOW"), UEmergeDisease::ImmunityLevel(0.30f), 3);
			TestEqual(TEXT("0.35 LOW (boundary)"), UEmergeDisease::ImmunityLevel(0.35f), 3);
			TestEqual(TEXT("0.50 MEDIUM"), UEmergeDisease::ImmunityLevel(0.50f), 2);
			TestEqual(TEXT("0.72 MEDIUM (boundary)"), UEmergeDisease::ImmunityLevel(0.72f), 2);
			TestEqual(TEXT("0.90 HIGH"), UEmergeDisease::ImmunityLevel(0.90f), 1);
			TestEqual(TEXT("0.95 HIGH (boundary)"), UEmergeDisease::ImmunityLevel(0.95f), 1);
			TestEqual(TEXT("0.98 GREAT"), UEmergeDisease::ImmunityLevel(0.98f), 0);
		});
	});

	Describe("AgentGrows", [this]()
	{
		It("grows only when potency is at or below the immune level", [this]()
		{
			// Salmonella/Cholera potency = HIGH = 1
			TestFalse(TEXT("HIGH potency vs GREAT immunity (0) -> dies"), UEmergeDisease::AgentGrows(1, 0));
			TestTrue(TEXT("HIGH potency vs HIGH immunity (1) -> grows"), UEmergeDisease::AgentGrows(1, 1));
			TestTrue(TEXT("HIGH potency vs MEDIUM immunity (2) -> grows"), UEmergeDisease::AgentGrows(1, 2));
			TestTrue(TEXT("HIGH potency vs CRITICAL immunity (4) -> grows"), UEmergeDisease::AgentGrows(1, 4));
		});
	});

	Describe("GrowDelta", [this]()
	{
		It("adds invasibility while growing, subtracts die-off otherwise (per tick)", [this]()
		{
			// Salmonella: invasibility 0.75, dieOffSpeed 1.0
			TestEqual(TEXT("salmonella grow dt=1 -> +0.75"), UEmergeDisease::GrowDelta(0.75f, 1.0f, 1, 2, 1.0f), 0.75f);
			TestEqual(TEXT("salmonella grow dt=2 -> +1.5"), UEmergeDisease::GrowDelta(0.75f, 1.0f, 1, 2, 2.0f), 1.5f);
			TestEqual(TEXT("salmonella die dt=1 -> -1.0"), UEmergeDisease::GrowDelta(0.75f, 1.0f, 1, 0, 1.0f), -1.0f);
			// Cholera: invasibility 0.15, dieOffSpeed 0.45
			TestEqual(TEXT("cholera grow dt=1 -> +0.15"), UEmergeDisease::GrowDelta(0.15f, 0.45f, 1, 4, 1.0f), 0.15f);
			TestEqual(TEXT("cholera die dt=1 -> -0.45"), UEmergeDisease::GrowDelta(0.15f, 0.45f, 1, 0, 1.0f), -0.45f);
		});
	});

	Describe("StepCount", [this]()
	{
		It("advances the agent count and clamps to [0, MaxCount]", [this]()
		{
			TestEqual(TEXT("salmonella growth"), UEmergeDisease::StepCount(100.f, 0.75f, 300.f), 100.75f);
			TestEqual(TEXT("cannot go below 0"), UEmergeDisease::StepCount(0.f, -1.0f, 300.f), 0.0f);
			TestEqual(TEXT("clamped at salmonella max 300"), UEmergeDisease::StepCount(299.5f, 0.75f, 300.f), 300.0f);
			TestEqual(TEXT("cholera die-off below max 1000"), UEmergeDisease::StepCount(1000.f, -0.45f, 1000.f), 999.55f);
		});
	});
}
#endif
