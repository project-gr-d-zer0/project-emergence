// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: rope/ladder climb stamina (survival/traversal).
// Climbing costs stamina proportional to height and takes time by climb speed; you can't start a climb without
// the stamina for it. Composes with EmergeStaminaModel. TUNABLE design costs, fixed deterministic model.
// CONTRACT — NEW class UEmergeRopeClimb : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeRopeClimb.h + Private/Survival/EmergeRopeClimb.cpp, static UFUNCTIONs.
//
//   static float StaminaCost(float HeightMeters, float PerMeterCost);   // return HeightMeters * PerMeterCost;
//   static bool  CanClimb(float CurrentStamina, float HeightMeters, float PerMeterCost);
//     // return CurrentStamina >= StaminaCost(HeightMeters, PerMeterCost);
//   static float ClimbTimeSec(float HeightMeters, float ClimbSpeedMps);   // return HeightMeters / ClimbSpeedMps;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeRopeClimb.h"

BEGIN_DEFINE_SPEC(FEmergeRopeClimbSpec, "Emergence.Survival.RopeClimb",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeRopeClimbSpec)
void FEmergeRopeClimbSpec::Define()
{
	Describe("Climb cost & feasibility", [this]()
	{
		It("stamina by height, gated, timed by speed", [this]()
		{
			TestEqual(TEXT("5m @2/m -> 10"), UEmergeRopeClimb::StaminaCost(5.f, 2.f), 10.0f);
			TestEqual(TEXT("10m @2/m -> 20"), UEmergeRopeClimb::StaminaCost(10.f, 2.f), 20.0f);
			TestTrue(TEXT("30 stamina can climb 5m"), UEmergeRopeClimb::CanClimb(30.f, 5.f, 2.f));
			TestFalse(TEXT("5 stamina cannot climb 5m"), UEmergeRopeClimb::CanClimb(5.f, 5.f, 2.f));
			TestEqual(TEXT("10m @2m/s -> 5s"), UEmergeRopeClimb::ClimbTimeSec(10.f, 2.f), 5.0f);
			TestEqual(TEXT("6m @2m/s -> 3s"), UEmergeRopeClimb::ClimbTimeSec(6.f, 2.f), 3.0f);
		});
	});
}
#endif
