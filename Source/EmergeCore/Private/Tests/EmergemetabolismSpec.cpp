// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hunger/thirst metabolism (DayZ-style).
// Closes the survival loop: movement burns energy AND water each tick at a basal+activity rate, shaped
// non-linearly by how full the stat is; once a stat drops to the low threshold it bleeds health. The energy
// and water outputs feed straight into the disease immunity formula (EmergeDisease::Immunity01), so hard
// activity -> depletion -> weak immunity -> agents grow. Numbers exact from decompiled DayZ: hunger.c/thirst.c
// OnTick, miscgameplayfunctions.c GetEnergy/WaterMetabolicSpeed, playerconstants.c (BASAL 0.01, WALK 0.1,
// JOG 0.3, SPRINT 0.6 for BOTH energy and water; SL_*_MAX 5000; CONSUMPTION_MULTIPLIER_BASE 1; SL_*_LOW 300;
// LOW_*_DAMAGE_PER_SEC 0.05).
// CONTRACT — NEW class UEmergeMetabolism : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeMetabolism.h + Private/Survival/EmergeMetabolism.cpp, static UFUNCTIONs.
//   Movement is an int32: 0 = Idle, 1 = Walk, 2 = Run, 3 = Sprint.
//
//   static float MetabolicSpeed(int32 Movement);
//     // Per-second base drain, identical constants for energy and water. Activity rate + basal 0.01.
//     // float s; switch(Movement){ case 1: s=0.1f; break; case 2: s=0.3f; break; case 3: s=0.6f; break; default: s=0.f; }
//     // return s + 0.01f;   // METABOLIC_SPEED_*_BASAL
//
//   static float ConsumptionModifier(float Stat);
//     // Non-linear shaping: fuller stat drains faster. return Stat / 5000.0f + 1.0f;  // /SL_*_MAX + CONSUMPTION_MULTIPLIER_BASE
//
//   static float DrainPerTick(int32 Movement, float Stat, float DeltaT);
//     // Amount subtracted from the stat this tick. return MetabolicSpeed(Movement) * ConsumptionModifier(Stat) * DeltaT;
//
//   static bool IsLow(float Stat);
//     // return Stat <= 300.0f;   // SL_*_LOW threshold from which the stat starts hurting health
//
//   static float LowStatHealthDamage(float Stat, float DeltaT);
//     // if (IsLow(Stat)) return 0.05f * DeltaT;   // LOW_*_DAMAGE_PER_SEC
//     // return 0.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeMetabolism.h"

BEGIN_DEFINE_SPEC(FEmergeMetabolismSpec, "Emergence.Survival.Metabolism",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMetabolismSpec)
void FEmergeMetabolismSpec::Define()
{
	Describe("MetabolicSpeed", [this]()
	{
		It("activity rate plus basal 0.01, for each movement state", [this]()
		{
			TestEqual(TEXT("idle -> 0.01 basal only"), UEmergeMetabolism::MetabolicSpeed(0), 0.01f);
			TestEqual(TEXT("walk -> 0.11"), UEmergeMetabolism::MetabolicSpeed(1), 0.11f);
			TestEqual(TEXT("run -> 0.31"), UEmergeMetabolism::MetabolicSpeed(2), 0.31f);
			TestEqual(TEXT("sprint -> 0.61"), UEmergeMetabolism::MetabolicSpeed(3), 0.61f);
			TestEqual(TEXT("unknown -> basal only"), UEmergeMetabolism::MetabolicSpeed(99), 0.01f);
		});
	});

	Describe("ConsumptionModifier", [this]()
	{
		It("stat/5000 + 1 (fuller drains faster)", [this]()
		{
			TestEqual(TEXT("full 5000 -> 2.0"), UEmergeMetabolism::ConsumptionModifier(5000.f), 2.0f);
			TestEqual(TEXT("half 2500 -> 1.5"), UEmergeMetabolism::ConsumptionModifier(2500.f), 1.5f);
			TestEqual(TEXT("empty 0 -> 1.0"), UEmergeMetabolism::ConsumptionModifier(0.f), 1.0f);
			TestEqual(TEXT("300 -> 1.06"), UEmergeMetabolism::ConsumptionModifier(300.f), 1.06f);
		});
	});

	Describe("DrainPerTick", [this]()
	{
		It("metabolic speed * consumption modifier * dt", [this]()
		{
			TestEqual(TEXT("sprint full 5000 dt=1 -> 1.22"), UEmergeMetabolism::DrainPerTick(3, 5000.f, 1.0f), 1.22f);
			TestEqual(TEXT("idle full 5000 dt=1 -> 0.02"), UEmergeMetabolism::DrainPerTick(0, 5000.f, 1.0f), 0.02f);
			TestEqual(TEXT("walk half 2500 dt=1 -> 0.165"), UEmergeMetabolism::DrainPerTick(1, 2500.f, 1.0f), 0.165f);
			TestEqual(TEXT("run empty 0 dt=1 -> 0.31"), UEmergeMetabolism::DrainPerTick(2, 0.f, 1.0f), 0.31f);
			TestEqual(TEXT("sprint full dt=2 -> 2.44"), UEmergeMetabolism::DrainPerTick(3, 5000.f, 2.0f), 2.44f);
		});
	});

	Describe("Low-stat health damage", [this]()
	{
		It("stat at/below 300 bleeds 0.05 hp/sec, above 300 none", [this]()
		{
			TestTrue(TEXT("300 is low (boundary)"), UEmergeMetabolism::IsLow(300.f));
			TestFalse(TEXT("301 not low"), UEmergeMetabolism::IsLow(301.f));
			TestTrue(TEXT("0 is low"), UEmergeMetabolism::IsLow(0.f));
			TestFalse(TEXT("500 not low"), UEmergeMetabolism::IsLow(500.f));
			TestEqual(TEXT("low 200 dt=1 -> 0.05"), UEmergeMetabolism::LowStatHealthDamage(200.f, 1.0f), 0.05f);
			TestEqual(TEXT("boundary 300 dt=1 -> 0.05"), UEmergeMetabolism::LowStatHealthDamage(300.f, 1.0f), 0.05f);
			TestEqual(TEXT("healthy 400 dt=1 -> 0.0"), UEmergeMetabolism::LowStatHealthDamage(400.f, 1.0f), 0.0f);
			TestEqual(TEXT("low 100 dt=2 -> 0.10"), UEmergeMetabolism::LowStatHealthDamage(100.f, 2.0f), 0.10f);
		});
	});
}
#endif
