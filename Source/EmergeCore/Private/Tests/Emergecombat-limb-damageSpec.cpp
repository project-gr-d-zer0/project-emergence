// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: limb damage multipliers + leg-injury
// locomotion tiers (research: Tarkov limb model — bruised/fractured/blacked tiers gate speed + sprint).
// CONTRACT — NEW class UEmergeLimbDamage : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeLimbDamage.h + Private/Combat/EmergeLimbDamage.cpp, static UFUNCTIONs:
//   static float MultiplierForLimb(int32 Limb);
//     // 0 head->2.5, 1 thorax->1.0, 2 stomach->1.2, 3 arm->0.7, 4 leg->0.65, else 1.0
//   static float SpeedMultiplierForLegState(int32 State);
//     // 0 fine->1.0, 1 bruised->0.85, 2 fractured->0.6, 3 and beyond blacked->0.4
//   static bool CanSprintWithLegState(int32 State);   // = State < 2
// Deterministic, exact golden values.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeLimbDamage.h"

BEGIN_DEFINE_SPEC(FEmergeLimbDamageSpec, "Emergence.Combat.LimbDamage",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeLimbDamageSpec)
void FEmergeLimbDamageSpec::Define()
{
	Describe("MultiplierForLimb", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("head 2.5"), UEmergeLimbDamage::MultiplierForLimb(0), 2.5f);
			TestEqual(TEXT("thorax 1.0"), UEmergeLimbDamage::MultiplierForLimb(1), 1.f);
			TestEqual(TEXT("stomach 1.2"), UEmergeLimbDamage::MultiplierForLimb(2), 1.2f);
			TestEqual(TEXT("arm 0.7"), UEmergeLimbDamage::MultiplierForLimb(3), 0.7f);
			TestEqual(TEXT("leg 0.65"), UEmergeLimbDamage::MultiplierForLimb(4), 0.65f);
			TestEqual(TEXT("unknown 1.0"), UEmergeLimbDamage::MultiplierForLimb(11), 1.f);
		});
	});
	Describe("leg state tiers", [this]()
	{
		It("speed multiplier per tier", [this]()
		{
			TestEqual(TEXT("fine 1.0"), UEmergeLimbDamage::SpeedMultiplierForLegState(0), 1.f);
			TestEqual(TEXT("bruised 0.85"), UEmergeLimbDamage::SpeedMultiplierForLegState(1), 0.85f);
			TestEqual(TEXT("fractured 0.6"), UEmergeLimbDamage::SpeedMultiplierForLegState(2), 0.6f);
			TestEqual(TEXT("blacked 0.4"), UEmergeLimbDamage::SpeedMultiplierForLegState(3), 0.4f);
			TestEqual(TEXT("beyond 0.4"), UEmergeLimbDamage::SpeedMultiplierForLegState(5), 0.4f);
		});
		It("sprint blocked from fractured onward", [this]()
		{
			TestTrue(TEXT("fine"), UEmergeLimbDamage::CanSprintWithLegState(0));
			TestTrue(TEXT("bruised"), UEmergeLimbDamage::CanSprintWithLegState(1));
			TestFalse(TEXT("fractured"), UEmergeLimbDamage::CanSprintWithLegState(2));
			TestFalse(TEXT("blacked"), UEmergeLimbDamage::CanSprintWithLegState(3));
		});
	});
}
#endif
