// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: foregrip recoil/handling (combat). [CORPUS-FIX]
// SOURCE-CORRECTED to Tarkov: spt items.json foregrips (n=56) give Recoil only -4..0% (mean -1.5%) and
// Ergonomics -8..+10 (mean +5.3). Foregrips are ERGONOMICS items, they barely touch recoil. Prior -20%/-15%/-50%
// recoil cuts exceeded the strongest real foregrip (-4%) by ~4-5x. Fix: recoil ~0.96-1.0; put the benefit on
// handling/ergonomics. (An always-on -50% bipod is not source-grounded; the big cut only applies to a DEPLOYED
// bipod at runtime, so undeployed bipod recoil = 1.0.)
// CONTRACT — NEW class UEmergeForegrip : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeForegrip.h + Private/Combat/EmergeForegrip.cpp, static UFUNCTIONs.
//   GripType int32: 0 None, 1 Vertical, 2 Angled, 3 Bipod (undeployed).
//
//   static float RecoilMultiplier(int32 GripType);
//     // switch: 1 -> 0.96f; 2 -> 0.98f; 3 -> 1.0f; default -> 1.0f;   // Tarkov foregrips only -1..-4% recoil
//   static float HandlingMultiplier(int32 GripType);
//     // switch: 1 -> 1.05f; 2 -> 1.1f; 3 -> 0.7f; default -> 1.0f;    // ergonomics up to +10; bipod hurts mobility
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeForegrip.h"

BEGIN_DEFINE_SPEC(FEmergeForegripSpec, "Emergence.Combat.Foregrip",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeForegripSpec)
void FEmergeForegripSpec::Define()
{
	Describe("Grip multipliers (Tarkov source ranges)", [this]()
	{
		It("recoil barely moves (-1..-4%); ergonomics is the real benefit", [this]()
		{
			TestEqual(TEXT("no grip recoil 1.0"), UEmergeForegrip::RecoilMultiplier(0), 1.0f);
			TestEqual(TEXT("vertical recoil 0.96 (-4%)"), UEmergeForegrip::RecoilMultiplier(1), 0.96f);
			TestEqual(TEXT("angled recoil 0.98 (-2%)"), UEmergeForegrip::RecoilMultiplier(2), 0.98f);
			TestEqual(TEXT("bipod undeployed recoil 1.0"), UEmergeForegrip::RecoilMultiplier(3), 1.0f);
			TestEqual(TEXT("angled handling +10%"), UEmergeForegrip::HandlingMultiplier(2), 1.1f);
			TestEqual(TEXT("vertical handling +5%"), UEmergeForegrip::HandlingMultiplier(1), 1.05f);
			TestEqual(TEXT("bipod hurts mobility 0.7"), UEmergeForegrip::HandlingMultiplier(3), 0.7f);
			TestEqual(TEXT("no grip handling 1.0"), UEmergeForegrip::HandlingMultiplier(0), 1.0f);
		});
	});
}
#endif
