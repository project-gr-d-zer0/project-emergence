// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: armor damage mitigation.
// Connects ballistics <-> faction armor: a round that fails to penetrate only bleeds a fraction
// through; a round that penetrates is still reduced by residual armor.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeBallistics (do NOT rename/duplicate it;
//   keep Penetrates + ComputeDamage exactly as they are), in the existing
//   Public/Combat/EmergeBallistics.h + Private/Combat/EmergeBallistics.cpp:
//   static float MitigatedDamage(float RawDamage, float ArmorValue, bool bPenetrated);
//     // if (!bPenetrated) return RawDamage * 0.15f;                              // 15% bleed-through
//     // const float Reduce = FMath::Clamp(ArmorValue / 200.0f, 0.0f, 0.6f);      // up to 60% residual
//     // return RawDamage * (1.0f - Reduce);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeBallistics.h"

BEGIN_DEFINE_SPEC(FEmergeBallisticsMitigationSpec, "Emergence.Ballistics.Mitigation",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBallisticsMitigationSpec)
void FEmergeBallisticsMitigationSpec::Define()
{
	Describe("MitigatedDamage (armor absorbs)", [this]()
	{
		It("bleeds 15% when stopped, reduces by residual armor when penetrated", [this]()
		{
			TestEqual(TEXT("stopped -> 15% bleed"),      UEmergeBallistics::MitigatedDamage(100.f, 80.f, false), 15.f);
			TestEqual(TEXT("pen w/80 armor -> 60"),      UEmergeBallistics::MitigatedDamage(100.f, 80.f, true), 60.f);
			TestEqual(TEXT("pen w/0 armor -> full"),     UEmergeBallistics::MitigatedDamage(100.f, 0.f, true), 100.f);
			TestEqual(TEXT("pen w/500 armor -> 40 (cap)"),UEmergeBallistics::MitigatedDamage(100.f, 500.f, true), 40.f);
		});
	});
}
#endif
