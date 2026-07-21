// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon jam chance (combat/gunplay).
// A worn or overheated weapon can malfunction: jam chance rises as weapon health drops and heat climbs, then a
// roll decides. Composes with the existing EmergeWeaponHeat. TUNABLE design weights, fixed deterministic model;
// the roll is passed in so the test stays deterministic (no RNG in asserts).
// CONTRACT — NEW class UEmergeWeaponJam : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeWeaponJam.h + Private/Combat/EmergeWeaponJam.cpp, static UFUNCTIONs.
//
//   static float JamChance(float WeaponHealth01, float Heat01);
//     // const float h = FMath::Clamp(WeaponHealth01, 0.0f, 1.0f);
//     // const float t = FMath::Clamp(Heat01, 0.0f, 1.0f);
//     // return FMath::Clamp((1.0f - h) * 0.1f + t * 0.05f, 0.0f, 1.0f);
//   static bool WouldJam(float JamRoll01, float Chance);   // return JamRoll01 < Chance;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeWeaponJam.h"

BEGIN_DEFINE_SPEC(FEmergeWeaponJamSpec, "Emergence.Combat.WeaponJam",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeaponJamSpec)
void FEmergeWeaponJamSpec::Define()
{
	Describe("JamChance", [this]()
	{
		It("rises with wear and heat", [this]()
		{
			TestEqual(TEXT("pristine cool -> 0"), UEmergeWeaponJam::JamChance(1.0f, 0.0f), 0.0f);
			TestEqual(TEXT("broken cool -> 0.1"), UEmergeWeaponJam::JamChance(0.0f, 0.0f), 0.1f);
			TestEqual(TEXT("pristine hot -> 0.05"), UEmergeWeaponJam::JamChance(1.0f, 1.0f), 0.05f);
			TestEqual(TEXT("half/half -> 0.075"), UEmergeWeaponJam::JamChance(0.5f, 0.5f), 0.075f);
			TestEqual(TEXT("broken hot -> 0.15"), UEmergeWeaponJam::JamChance(0.0f, 1.0f), 0.15f);
		});
	});

	Describe("WouldJam", [this]()
	{
		It("jams when the roll falls under the chance", [this]()
		{
			TestTrue(TEXT("roll 0.05 under 0.1"), UEmergeWeaponJam::WouldJam(0.05f, 0.1f));
			TestFalse(TEXT("roll 0.15 over 0.1"), UEmergeWeaponJam::WouldJam(0.15f, 0.1f));
			TestFalse(TEXT("roll equal not under"), UEmergeWeaponJam::WouldJam(0.1f, 0.1f));
		});
	});
}
#endif
