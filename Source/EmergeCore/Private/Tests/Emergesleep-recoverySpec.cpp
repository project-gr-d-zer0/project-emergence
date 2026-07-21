// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: sleep / rest recovery (survival).
// Resting and sleeping restore energy and burn down fatigue faster than being active - the recovery side of the
// EmergeMetabolism/stamina economy. TUNABLE design rates, fixed deterministic model.
// CONTRACT — NEW class UEmergeSleepRecovery : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeSleepRecovery.h + Private/Survival/EmergeSleepRecovery.cpp, static UFUNCTIONs.
//   RestState int32: 0 Active, 1 Resting, 2 Sleeping.
//
//   static float EnergyRestorePerTick(int32 RestState, float DeltaT);
//     // switch: 2 -> 3.0f; 1 -> 1.0f; default -> 0.0f;  then * DeltaT
//   static float FatigueRecoverPerTick(int32 RestState, float DeltaT);
//     // switch: 2 -> 5.0f; 1 -> 2.0f; default -> 0.0f;  then * DeltaT
//   static bool  IsResting(int32 RestState);   // return RestState >= 1;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeSleepRecovery.h"

BEGIN_DEFINE_SPEC(FEmergeSleepRecoverySpec, "Emergence.Survival.SleepRecovery",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSleepRecoverySpec)
void FEmergeSleepRecoverySpec::Define()
{
	Describe("Recovery rates", [this]()
	{
		It("sleeping restores fastest, active not at all", [this]()
		{
			TestEqual(TEXT("sleeping energy 3/s"), UEmergeSleepRecovery::EnergyRestorePerTick(2, 1.0f), 3.0f);
			TestEqual(TEXT("resting energy 1/s"), UEmergeSleepRecovery::EnergyRestorePerTick(1, 1.0f), 1.0f);
			TestEqual(TEXT("active energy 0"), UEmergeSleepRecovery::EnergyRestorePerTick(0, 1.0f), 0.0f);
			TestEqual(TEXT("sleeping dt=2 -> 6"), UEmergeSleepRecovery::EnergyRestorePerTick(2, 2.0f), 6.0f);
			TestEqual(TEXT("sleeping fatigue 5/s"), UEmergeSleepRecovery::FatigueRecoverPerTick(2, 1.0f), 5.0f);
			TestEqual(TEXT("resting fatigue 2/s"), UEmergeSleepRecovery::FatigueRecoverPerTick(1, 1.0f), 2.0f);
			TestEqual(TEXT("active fatigue 0"), UEmergeSleepRecovery::FatigueRecoverPerTick(0, 1.0f), 0.0f);
		});
	});

	Describe("IsResting", [this]()
	{
		It("resting or sleeping counts as resting", [this]()
		{
			TestFalse(TEXT("active not resting"), UEmergeSleepRecovery::IsResting(0));
			TestTrue(TEXT("resting"), UEmergeSleepRecovery::IsResting(1));
			TestTrue(TEXT("sleeping"), UEmergeSleepRecovery::IsResting(2));
		});
	});
}
#endif
