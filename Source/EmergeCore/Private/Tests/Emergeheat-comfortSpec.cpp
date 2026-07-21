// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: heat comfort (DayZ-style thermoregulation).
// A signed heat-comfort value in [-1,+1] (0 = comfortable) driven by clothing insulation + environment;
// it moves toward its target by a clamped step each tick, and crosses warning/critical/extreme zones on
// both the hot (+) and cold (-) sides. Composes with the kit's modular clothing (per-slot insulation
// weights). Numbers exact from decompiled DayZ playerconstants.c/constants.c: zone thresholds
// +/-0.15/0.45/0.75, max dynamic step 0.25, per-body-part heatcomfort weights.
// CONTRACT — NEW class UEmergeHeatComfort : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeHeatComfort.h + Private/Survival/EmergeHeatComfort.cpp, static UFUNCTIONs:
//
//   static int32 ComfortZone(float HeatComfort);
//     // Signed zone: hot +1/+2/+3 (warning/critical/extreme), cold -1/-2/-3, 0 = comfortable.
//     // if (HeatComfort >= 0.75f) return 3;  if (HeatComfort >= 0.45f) return 2;  if (HeatComfort >= 0.15f) return 1;
//     // if (HeatComfort <= -0.75f) return -3; if (HeatComfort <= -0.45f) return -2; if (HeatComfort <= -0.15f) return -1;
//     // return 0;
//
//   static float StepTowards(float Current, float Target);
//     // Move Current toward Target by at most 0.25 (ENVIRO_HEATCOMFORT_MAX_STEP_SIZE).
//     // const float Diff = Target - Current;
//     // if (FMath::Abs(Diff) <= 0.25f) return Target;
//     // return Current + (Diff > 0.f ? 0.25f : -0.25f);
//
//   static float BodyPartWeight(int32 Slot);
//     // Per-slot contribution to final heat comfort. Slot: 0=Head->0.05, 1=Mask->0.05, 2=Vest->0.04,
//     // 3=Body->0.12, 4=Back->0.03, 5=Gloves->0.04, 6=Legs->0.12, 7=Feet->0.06, 8=Hips->0.0; other -> 0.0.
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeHeatComfort.h"

BEGIN_DEFINE_SPEC(FEmergeHeatComfortSpec, "Emergence.Survival.HeatComfort",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHeatComfortSpec)
void FEmergeHeatComfortSpec::Define()
{
	Describe("ComfortZone", [this]()
	{
		It("classifies hot/cold warning/critical/extreme zones (exact thresholds)", [this]()
		{
			TestEqual(TEXT("0 comfortable"), UEmergeHeatComfort::ComfortZone(0.f), 0);
			TestEqual(TEXT("0.14 still comfortable"), UEmergeHeatComfort::ComfortZone(0.14f), 0);
			TestEqual(TEXT("0.15 hot warning"), UEmergeHeatComfort::ComfortZone(0.15f), 1);
			TestEqual(TEXT("0.45 hot critical"), UEmergeHeatComfort::ComfortZone(0.45f), 2);
			TestEqual(TEXT("0.75 hot extreme"), UEmergeHeatComfort::ComfortZone(0.75f), 3);
			TestEqual(TEXT("-0.15 cold warning"), UEmergeHeatComfort::ComfortZone(-0.15f), -1);
			TestEqual(TEXT("-0.45 cold critical"), UEmergeHeatComfort::ComfortZone(-0.45f), -2);
			TestEqual(TEXT("-0.75 cold extreme"), UEmergeHeatComfort::ComfortZone(-0.75f), -3);
		});
	});

	Describe("StepTowards", [this]()
	{
		It("clamped 0.25 step toward the target", [this]()
		{
			TestEqual(TEXT("far up -> +0.25"), UEmergeHeatComfort::StepTowards(0.f, 1.0f), 0.25f);
			TestEqual(TEXT("far down -> -0.25"), UEmergeHeatComfort::StepTowards(0.5f, 0.f), 0.25f);
			TestEqual(TEXT("within step snaps to target"), UEmergeHeatComfort::StepTowards(0.f, 0.1f), 0.1f);
			TestEqual(TEXT("exactly 0.25 snaps"), UEmergeHeatComfort::StepTowards(0.f, 0.25f), 0.25f);
			TestEqual(TEXT("cold far -> -0.25"), UEmergeHeatComfort::StepTowards(0.f, -1.0f), -0.25f);
		});
	});

	Describe("BodyPartWeight", [this]()
	{
		It("per-slot heatcomfort weights (exact DayZ values)", [this]()
		{
			TestEqual(TEXT("head 0.05"), UEmergeHeatComfort::BodyPartWeight(0), 0.05f);
			TestEqual(TEXT("body 0.12"), UEmergeHeatComfort::BodyPartWeight(3), 0.12f);
			TestEqual(TEXT("legs 0.12"), UEmergeHeatComfort::BodyPartWeight(6), 0.12f);
			TestEqual(TEXT("hips 0.0"), UEmergeHeatComfort::BodyPartWeight(8), 0.0f);
			TestEqual(TEXT("unknown 0.0"), UEmergeHeatComfort::BodyPartWeight(99), 0.0f);
		});
	});
}
#endif
