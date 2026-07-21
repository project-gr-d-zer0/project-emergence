// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: survivor tick INTEGRATION (survival).
// THE FIRST VERTICAL-DEPTH MODULE: instead of another isolated formula, this COMPOSES existing modules into a
// one-tick survivor advance - EmergeMetabolism (energy/water drain), EmergeBleeding (blood loss), EmergeDisease
// (immunity from vitals) - the missing "middle rung" that wires the sim together. Tested MOSTLY METAMORPHICALLY
// (relations across runs), not fragile exact goldens: this is robust to float precision and actually verifies the
// COMPOSITION behaves right (sprinting drains more, more wounds bleed more, immunity tracks vitals) rather than
// re-checking a leaf formula.
// CONTRACT — NEW class UEmergeSurvivorTick : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeSurvivorTick.h + Private/Survival/EmergeSurvivorTick.cpp, static UFUNCTIONs.
//   The .cpp must #include "Survival/EmergeMetabolism.h", "Survival/EmergeBleeding.h", "Survival/EmergeDisease.h".
//   Movement int32 0 Idle..3 Sprint (as EmergeMetabolism); bleeding Intensity 0 Low..2 High (as EmergeBleeding).
//
//   static float DrainedEnergy(float Energy, int32 Movement, float DeltaT);
//     // return FMath::Clamp(Energy - UEmergeMetabolism::DrainPerTick(Movement, Energy, DeltaT), 0.0f, 5000.0f);
//   static float DrainedBlood(float Blood, int32 Intensity, int32 Sources, float DeltaT);
//     // return FMath::Clamp(Blood + Sources * UEmergeBleeding::SourceBloodLossPerTick(Intensity, Blood, DeltaT), 0.0f, 5000.0f);
//   static int32 ImmunityFromState(float Energy, float Water, float Health01, float Blood01);
//     // return UEmergeDisease::ImmunityLevel(UEmergeDisease::Immunity01(Energy, Water, Health01, Blood01));
//   static bool IsDeteriorating(float EnergyBefore, float EnergyAfter, float BloodBefore, float BloodAfter);
//     // return (EnergyAfter < EnergyBefore) || (BloodAfter < BloodBefore);
//
// Deterministic; a few tolerance-safe exact goldens + metamorphic relations.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeSurvivorTick.h"

BEGIN_DEFINE_SPEC(FEmergeSurvivorTickSpec, "Emergence.Survival.SurvivorTick",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSurvivorTickSpec)
void FEmergeSurvivorTickSpec::Define()
{
	Describe("Energy drain composition", [this]()
	{
		It("exact (tolerance) + metamorphic: sprint drains more than idle, stays bounded", [this]()
		{
			// exact, with tolerance (float-safe)
			TestEqual(TEXT("sprint full dt=1"), UEmergeSurvivorTick::DrainedEnergy(5000.f, 3, 1.f), 4998.78f, 0.01f);
			TestEqual(TEXT("idle full dt=1"), UEmergeSurvivorTick::DrainedEnergy(5000.f, 0, 1.f), 4999.98f, 0.01f);
			// METAMORPHIC relation: harder movement leaves less energy
			const float sprint = UEmergeSurvivorTick::DrainedEnergy(5000.f, 3, 1.f);
			const float idle   = UEmergeSurvivorTick::DrainedEnergy(5000.f, 0, 1.f);
			TestTrue(TEXT("sprint < idle (drains more)"), sprint < idle);
			// bounded over a sweep
			for (int32 e = 0; e <= 5000; e += 500)
			{
				const float n = UEmergeSurvivorTick::DrainedEnergy((float)e, 3, 1.f);
				TestTrue(TEXT("energy stays in [0,5000]"), n >= 0.f && n <= 5000.f);
			}
		});
	});

	Describe("Blood loss composition", [this]()
	{
		It("metamorphic: more sources bleed more; bounded; exact at full", [this]()
		{
			TestEqual(TEXT("3 high sources full blood dt=1 -> 4940"), UEmergeSurvivorTick::DrainedBlood(5000.f, 2, 3, 1.f), 4940.0f, 0.01f);
			const float three = UEmergeSurvivorTick::DrainedBlood(5000.f, 2, 3, 1.f);
			const float one   = UEmergeSurvivorTick::DrainedBlood(5000.f, 2, 1, 1.f);
			TestTrue(TEXT("more sources -> less blood"), three < one);
			for (int32 b = 0; b <= 5000; b += 500)
			{
				const float n = UEmergeSurvivorTick::DrainedBlood((float)b, 2, 3, 1.f);
				TestTrue(TEXT("blood stays in [0,5000]"), n >= 0.f && n <= 5000.f);
			}
		});
	});

	Describe("Immunity composition & deterioration", [this]()
	{
		It("immunity tracks vitals (monotonic); deterioration flag", [this]()
		{
			TestEqual(TEXT("full vitals -> GREAT (0)"), UEmergeSurvivorTick::ImmunityFromState(5000.f, 5000.f, 1.f, 1.f), 0);
			// METAMORPHIC: higher energy never worsens immunity level (lower ordinal = better)
			int32 prev = 5;
			for (int32 e = 0; e <= 5000; e += 500)
			{
				const int32 lvl = UEmergeSurvivorTick::ImmunityFromState((float)e, 2500.f, 0.5f, 0.5f);
				TestTrue(TEXT("more energy -> not-worse immunity"), lvl <= prev);
				prev = lvl;
			}
			TestTrue(TEXT("losing energy = deteriorating"), UEmergeSurvivorTick::IsDeteriorating(5000.f, 4998.f, 5000.f, 5000.f));
			TestFalse(TEXT("stable = not deteriorating"), UEmergeSurvivorTick::IsDeteriorating(5000.f, 5000.f, 5000.f, 5000.f));
		});
	});
}
#endif
