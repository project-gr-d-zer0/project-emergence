// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon jam chance (combat/gunplay). [CORPUS-FIX]
// SOURCE-CORRECTED to EFT: spt globals.json config.Overheat: MinMalfChance 0.5(%), MaxMalfChance 9(%),
// OverheatProblemsStart 100, MaxOverheat 200. Prior model ranged 0..15% unbounded-ish; real EFT malfunction is
// bounded 0.5%..9%, driven by weapon wear (durability) + overheat, and heat only contributes ABOVE the 100/200
// overheat border (Heat01 > 0.5). The roll is passed in so the test stays deterministic.
// CONTRACT — NEW class UEmergeWeaponJam : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeWeaponJam.h + Private/Combat/EmergeWeaponJam.cpp, static UFUNCTIONs.
//
//   static float JamChance(float WeaponHealth01, float Heat01);
//     // const float wear = 1.0f - FMath::Clamp(WeaponHealth01, 0.0f, 1.0f);
//     // const float heatOver = FMath::Clamp((FMath::Clamp(Heat01, 0.0f, 1.0f) - 0.5f) / 0.5f, 0.0f, 1.0f); // problems start at 100/200
//     // const float raw = 0.005f + wear * 0.06f + heatOver * 0.025f;   // MinMalfChance 0.5% floor; wear+overheat push to 9%
//     // return FMath::Clamp(raw, 0.005f, 0.09f);   // MaxMalfChance 9%
//   static bool WouldJam(float JamRoll01, float Chance);   // return JamRoll01 < Chance;
//
// Deterministic, exact golden values (tolerance-safe), boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeWeaponJam.h"

BEGIN_DEFINE_SPEC(FEmergeWeaponJamSpec, "Emergence.Combat.WeaponJam",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeaponJamSpec)
void FEmergeWeaponJamSpec::Define()
{
	Describe("JamChance (EFT bounds 0.5%..9%)", [this]()
	{
		It("floors at 0.5%, caps at 9%, heat only past overheat 100/200", [this]()
		{
			TestEqual(TEXT("pristine cool -> 0.5% floor"), UEmergeWeaponJam::JamChance(1.0f, 0.0f), 0.005f, 1e-4f);
			TestEqual(TEXT("broken cool -> 6.5%"), UEmergeWeaponJam::JamChance(0.0f, 0.0f), 0.065f, 1e-4f);
			TestEqual(TEXT("pristine hot -> 3.0% (heat past 0.5)"), UEmergeWeaponJam::JamChance(1.0f, 1.0f), 0.03f, 1e-4f);
			TestEqual(TEXT("pristine half-heat -> floor (heat not past 0.5)"), UEmergeWeaponJam::JamChance(1.0f, 0.5f), 0.005f, 1e-4f);
			TestEqual(TEXT("broken hot -> 9% cap"), UEmergeWeaponJam::JamChance(0.0f, 1.0f), 0.09f, 1e-4f);
		});
	});

	Describe("WouldJam", [this]()
	{
		It("jams when the roll falls under the chance", [this]()
		{
			TestTrue(TEXT("roll 0.01 under 0.065"), UEmergeWeaponJam::WouldJam(0.01f, 0.065f));
			TestFalse(TEXT("roll 0.1 over 0.09"), UEmergeWeaponJam::WouldJam(0.1f, 0.09f));
		});
	});
}
#endif
