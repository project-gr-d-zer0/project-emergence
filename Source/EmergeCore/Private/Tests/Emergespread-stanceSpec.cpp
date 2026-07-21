// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon spread by stance (combat/gunplay).
// Effective bullet spread scales a weapon's base spread by stance (standing/crouch/prone tighten it), a moving
// penalty, and an ADS bonus - the accuracy model behind hip-fire vs aimed, moving vs planted. TUNABLE design
// multipliers, fixed deterministic model. Distinct from the existing EmergeWeaponSpread (per-shot cone) - this is
// the stance/movement modifier stack. Pure math.
// CONTRACT — NEW class UEmergeSpreadStance : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeSpreadStance.h + Private/Combat/EmergeSpreadStance.cpp, static UFUNCTIONs.
//   Stance int32: 0 Standing, 1 Crouched, 2 Prone.
//
//   static float StanceMultiplier(int32 Stance);
//     // switch: 1 -> 0.7f; 2 -> 0.5f; default (0/other) -> 1.0f;
//   static float EffectiveSpreadMoa(float BaseMoa, int32 Stance, bool bMoving, bool bAds);
//     // return BaseMoa * StanceMultiplier(Stance) * (bMoving ? 1.5f : 1.0f) * (bAds ? 0.5f : 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeSpreadStance.h"

BEGIN_DEFINE_SPEC(FEmergeSpreadStanceSpec, "Emergence.Combat.SpreadStance",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSpreadStanceSpec)
void FEmergeSpreadStanceSpec::Define()
{
	Describe("StanceMultiplier", [this]()
	{
		It("crouch and prone tighten spread", [this]()
		{
			TestEqual(TEXT("standing 1.0"), UEmergeSpreadStance::StanceMultiplier(0), 1.0f);
			TestEqual(TEXT("crouched 0.7"), UEmergeSpreadStance::StanceMultiplier(1), 0.7f);
			TestEqual(TEXT("prone 0.5"), UEmergeSpreadStance::StanceMultiplier(2), 0.5f);
			TestEqual(TEXT("unknown 1.0"), UEmergeSpreadStance::StanceMultiplier(9), 1.0f);
		});
	});

	Describe("EffectiveSpreadMoa", [this]()
	{
		It("stacks stance, movement penalty, and ADS bonus", [this]()
		{
			TestEqual(TEXT("standing hipfire planted"), UEmergeSpreadStance::EffectiveSpreadMoa(4.f, 0, false, false), 4.0f);
			TestEqual(TEXT("crouch ADS planted -> 1.4"), UEmergeSpreadStance::EffectiveSpreadMoa(4.f, 1, false, true), 1.4f);
			TestEqual(TEXT("prone moving hipfire -> 3.0"), UEmergeSpreadStance::EffectiveSpreadMoa(4.f, 2, true, false), 3.0f);
			TestEqual(TEXT("standing moving ADS -> 3.0"), UEmergeSpreadStance::EffectiveSpreadMoa(4.f, 0, true, true), 3.0f);
		});
	});
}
#endif
