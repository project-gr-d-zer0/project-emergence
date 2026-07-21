// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: foregrip recoil/handling (combat/gunplay).
// A foregrip attachment scales recoil (EmergeRecoil/EmergeMuzzleClimb) and weapon handling: vertical grip cuts
// recoil most, angled grip aids handling, bipod slashes recoil but hurts mobility. TUNABLE design multipliers.
// CONTRACT — NEW class UEmergeForegrip : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeForegrip.h + Private/Combat/EmergeForegrip.cpp, static UFUNCTIONs.
//   GripType int32: 0 None, 1 Vertical, 2 Angled, 3 Bipod.
//
//   static float RecoilMultiplier(int32 GripType);
//     // switch: 1 -> 0.8f; 2 -> 0.85f; 3 -> 0.5f; default -> 1.0f;
//   static float HandlingMultiplier(int32 GripType);
//     // switch: 1 -> 0.95f; 2 -> 1.1f; 3 -> 0.7f; default -> 1.0f;
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
	Describe("Grip multipliers", [this]()
	{
		It("recoil and handling per grip type", [this]()
		{
			TestEqual(TEXT("no grip recoil 1.0"), UEmergeForegrip::RecoilMultiplier(0), 1.0f);
			TestEqual(TEXT("vertical recoil 0.8"), UEmergeForegrip::RecoilMultiplier(1), 0.8f);
			TestEqual(TEXT("angled recoil 0.85"), UEmergeForegrip::RecoilMultiplier(2), 0.85f);
			TestEqual(TEXT("bipod recoil 0.5"), UEmergeForegrip::RecoilMultiplier(3), 0.5f);
			TestEqual(TEXT("angled handling 1.1"), UEmergeForegrip::HandlingMultiplier(2), 1.1f);
			TestEqual(TEXT("bipod handling 0.7"), UEmergeForegrip::HandlingMultiplier(3), 0.7f);
			TestEqual(TEXT("no grip handling 1.0"), UEmergeForegrip::HandlingMultiplier(0), 1.0f);
		});
	});
}
#endif
