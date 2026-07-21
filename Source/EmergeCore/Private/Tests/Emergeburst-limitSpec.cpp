// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: burst count limit (combat/gunplay).
// Burst fire-mode releases exactly N rounds per trigger pull then stops until released; this tracks how many
// rounds a pull produces and when the burst is complete. Pairs with EmergeFireMode. Pure counting; deterministic.
// CONTRACT — NEW class UEmergeBurstLimit : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeBurstLimit.h + Private/Combat/EmergeBurstLimit.cpp, static UFUNCTIONs.
//
//   static int32 RoundsThisBurst(int32 TriggerHeldRounds, int32 BurstSize);   // return FMath::Min(TriggerHeldRounds, BurstSize);
//   static bool  BurstComplete(int32 Fired, int32 BurstSize);                  // return Fired >= BurstSize;
//   static int32 RemainingInBurst(int32 Fired, int32 BurstSize);              // return FMath::Max(0, BurstSize - Fired);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeBurstLimit.h"

BEGIN_DEFINE_SPEC(FEmergeBurstLimitSpec, "Emergence.Combat.BurstLimit",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBurstLimitSpec)
void FEmergeBurstLimitSpec::Define()
{
	Describe("Burst counting", [this]()
	{
		It("caps a pull at the burst size and tracks completion", [this]()
		{
			TestEqual(TEXT("held past burst -> 3"), UEmergeBurstLimit::RoundsThisBurst(5, 3), 3);
			TestEqual(TEXT("released early -> 2"), UEmergeBurstLimit::RoundsThisBurst(2, 3), 2);
			TestTrue(TEXT("3 fired completes 3-burst"), UEmergeBurstLimit::BurstComplete(3, 3));
			TestFalse(TEXT("2 fired incomplete"), UEmergeBurstLimit::BurstComplete(2, 3));
			TestEqual(TEXT("1 fired -> 2 left"), UEmergeBurstLimit::RemainingInBurst(1, 3), 2);
			TestEqual(TEXT("overfired -> 0 left"), UEmergeBurstLimit::RemainingInBurst(5, 3), 0);
		});
	});
}
#endif
