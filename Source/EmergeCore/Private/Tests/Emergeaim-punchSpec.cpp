// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: aim punch on hit (combat/gunplay).
// Taking fire punches your view/aim proportional to the damage (capped), then it recovers over time - the
// "getting shot throws your aim off" feel. Composes with EmergeSuppression. TUNABLE design model, fixed.
// CONTRACT — NEW class UEmergeAimPunch : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeAimPunch.h + Private/Combat/EmergeAimPunch.cpp, static UFUNCTIONs.
//
//   static float PunchMagnitude(float Damage, float PunchPerDamage, float MaxPunch);
//     // return FMath::Min(Damage * PunchPerDamage, MaxPunch);
//   static float RecoverPunch(float Current, float RecoverRate, float DeltaT);
//     // return FMath::Max(0.0f, Current - RecoverRate * DeltaT);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeAimPunch.h"

BEGIN_DEFINE_SPEC(FEmergeAimPunchSpec, "Emergence.Combat.AimPunch",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAimPunchSpec)
void FEmergeAimPunchSpec::Define()
{
	Describe("PunchMagnitude & recovery", [this]()
	{
		It("bigger hits punch harder (capped), then recover", [this]()
		{
			TestEqual(TEXT("10 dmg @0.1 -> 1.0"), UEmergeAimPunch::PunchMagnitude(10.f, 0.1f, 5.f), 1.0f);
			TestEqual(TEXT("100 dmg caps at 5"), UEmergeAimPunch::PunchMagnitude(100.f, 0.1f, 5.f), 5.0f);
			TestEqual(TEXT("no damage -> 0"), UEmergeAimPunch::PunchMagnitude(0.f, 0.1f, 5.f), 0.0f);
			TestEqual(TEXT("5 recovers by 2 -> 3"), UEmergeAimPunch::RecoverPunch(5.f, 2.f, 1.f), 3.0f);
			TestEqual(TEXT("small recovers to 0"), UEmergeAimPunch::RecoverPunch(1.f, 2.f, 1.f), 0.0f);
		});
	});
}
#endif
