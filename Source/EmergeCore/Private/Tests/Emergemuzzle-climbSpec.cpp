// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: muzzle climb control (combat/gunplay).
// Distinct from EmergeRecoil (per-shot kick): sustained automatic fire makes the muzzle CLIMB progressively;
// grip/control reduces it and it settles between bursts. TUNABLE design model, fixed deterministic.
// CONTRACT — NEW class UEmergeMuzzleClimb : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeMuzzleClimb.h + Private/Combat/EmergeMuzzleClimb.cpp, static UFUNCTIONs.
//
//   static float ClimbAtShot(int32 ShotIndex, float ClimbPerShot, float GripControl01, float MaxClimb);
//     // const float g = FMath::Clamp(GripControl01, 0.0f, 1.0f);
//     // return FMath::Min(ShotIndex * ClimbPerShot * (1.0f - g), MaxClimb);
//   static float SettlePerTick(float Current, float SettleRate, float DeltaT);
//     // return FMath::Max(0.0f, Current - SettleRate * DeltaT);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeMuzzleClimb.h"

BEGIN_DEFINE_SPEC(FEmergeMuzzleClimbSpec, "Emergence.Combat.MuzzleClimb",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMuzzleClimbSpec)
void FEmergeMuzzleClimbSpec::Define()
{
	Describe("ClimbAtShot", [this]()
	{
		It("climbs with sustained fire, reduced by grip, capped", [this]()
		{
			TestEqual(TEXT("first shot no climb"), UEmergeMuzzleClimb::ClimbAtShot(0, 0.5f, 0.f, 10.f), 0.0f);
			TestEqual(TEXT("4 shots no grip -> 2.0"), UEmergeMuzzleClimb::ClimbAtShot(4, 0.5f, 0.f, 10.f), 2.0f);
			TestEqual(TEXT("caps at max"), UEmergeMuzzleClimb::ClimbAtShot(100, 0.5f, 0.f, 10.f), 10.0f);
			TestEqual(TEXT("grip halves climb"), UEmergeMuzzleClimb::ClimbAtShot(4, 0.5f, 0.5f, 10.f), 1.0f);
		});
	});

	Describe("SettlePerTick", [this]()
	{
		It("settles back down between bursts", [this]()
		{
			TestEqual(TEXT("5 settles by 2 -> 3"), UEmergeMuzzleClimb::SettlePerTick(5.f, 2.f, 1.f), 3.0f);
			TestEqual(TEXT("small settles to 0"), UEmergeMuzzleClimb::SettlePerTick(1.f, 2.f, 1.f), 0.0f);
		});
	});
}
#endif
