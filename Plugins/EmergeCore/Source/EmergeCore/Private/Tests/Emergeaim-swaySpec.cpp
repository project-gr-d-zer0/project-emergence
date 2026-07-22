// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: aim sway & hold-breath (combat/gunplay).
// Scoped sway grows as stamina drains and shrinks sharply while holding breath - the classic "steady the shot"
// mechanic. You can only hold breath with some stamina left. The hold-breath activation floor is exact from
// decompiled DayZ constants.c (STAMINA_HOLD_BREATH_THRESHOLD_ACTIVATE 10, of STAMINA_MAX 100 -> 0.1 normalized);
// the sway multipliers are tunable design values.
// CONTRACT — NEW class UEmergeAimSway : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeAimSway.h + Private/Combat/EmergeAimSway.cpp, static UFUNCTIONs.
//
//   static float SwayAmplitude(float BaseSway, bool bHoldingBreath, float Stamina01);
//     // const float s = FMath::Clamp(Stamina01, 0.0f, 1.0f);
//     // return BaseSway * (bHoldingBreath ? 0.3f : 1.0f) * (1.0f + (1.0f - s) * 0.5f);
//   static bool CanHoldBreath(float Stamina01);   // return Stamina01 > 0.1f;  // THRESHOLD_ACTIVATE 10/100
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeAimSway.h"

BEGIN_DEFINE_SPEC(FEmergeAimSwaySpec, "Emergence.Combat.AimSway",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAimSwaySpec)
void FEmergeAimSwaySpec::Define()
{
	Describe("SwayAmplitude", [this]()
	{
		It("full stamina baseline, breath steadies, low stamina worsens", [this]()
		{
			TestEqual(TEXT("full stamina, breathing -> base"), UEmergeAimSway::SwayAmplitude(1.0f, false, 1.0f), 1.0f);
			TestEqual(TEXT("full stamina, holding -> 0.3"), UEmergeAimSway::SwayAmplitude(1.0f, true, 1.0f), 0.3f);
			TestEqual(TEXT("empty stamina, breathing -> 1.5"), UEmergeAimSway::SwayAmplitude(1.0f, false, 0.0f), 1.5f);
			TestEqual(TEXT("empty stamina, holding -> 0.45"), UEmergeAimSway::SwayAmplitude(1.0f, true, 0.0f), 0.45f);
		});
	});

	Describe("CanHoldBreath", [this]()
	{
		It("needs stamina above the 0.1 activation floor", [this]()
		{
			TestTrue(TEXT("0.5 can hold"), UEmergeAimSway::CanHoldBreath(0.5f));
			TestFalse(TEXT("0.1 cannot (not strictly above)"), UEmergeAimSway::CanHoldBreath(0.1f));
			TestFalse(TEXT("0.05 cannot"), UEmergeAimSway::CanHoldBreath(0.05f));
		});
	});
}
#endif
