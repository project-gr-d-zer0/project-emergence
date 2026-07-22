// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: headshot mitigation by helmet (combat/gunplay).
// A helmet cuts headshot damage by its armor class and can stop a low-penetration round from a lethal headshot.
// Composes with EmergeHitZone (head multiplier) and EmergeArmorPen (class*10 threshold). TUNABLE design model.
// CONTRACT — NEW class UEmergeHeadshotHelmet : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeHeadshotHelmet.h + Private/Combat/EmergeHeadshotHelmet.cpp, static UFUNCTIONs.
//
//   static float HeadshotMultiplier(float BaseHeadMult, int32 HelmetClass);
//     // reduce by 15% per class, capped at 90%: const float r = FMath::Clamp(HelmetClass * 0.15f, 0.0f, 0.9f);
//     // return BaseHeadMult * (1.0f - r);
//   static bool StopsHeadshotKill(int32 HelmetClass, float PenPower);
//     // return PenPower < (HelmetClass * 10.0f);   // below the class threshold the helmet stops it
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeHeadshotHelmet.h"

BEGIN_DEFINE_SPEC(FEmergeHeadshotHelmetSpec, "Emergence.Combat.HeadshotHelmet",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHeadshotHelmetSpec)
void FEmergeHeadshotHelmetSpec::Define()
{
	Describe("HeadshotMultiplier", [this]()
	{
		It("helmet class cuts headshot damage, capped at 90%", [this]()
		{
			TestEqual(TEXT("no helmet -> full"), UEmergeHeadshotHelmet::HeadshotMultiplier(2.0f, 0), 2.0f);
			TestEqual(TEXT("class 2 -> 1.4"), UEmergeHeadshotHelmet::HeadshotMultiplier(2.0f, 2), 1.4f);
			TestEqual(TEXT("class 4 -> 0.8"), UEmergeHeadshotHelmet::HeadshotMultiplier(2.0f, 4), 0.8f);
			TestEqual(TEXT("class 6 caps at 90% -> 0.2"), UEmergeHeadshotHelmet::HeadshotMultiplier(2.0f, 6), 0.2f);
		});
	});

	Describe("StopsHeadshotKill", [this]()
	{
		It("helmet stops rounds under its class threshold", [this]()
		{
			TestTrue(TEXT("30 pen stopped by class 4"), UEmergeHeadshotHelmet::StopsHeadshotKill(4, 30.f));
			TestFalse(TEXT("45 pen defeats class 4"), UEmergeHeadshotHelmet::StopsHeadshotKill(4, 45.f));
			TestFalse(TEXT("no helmet stops nothing"), UEmergeHeadshotHelmet::StopsHeadshotKill(0, 5.f));
		});
	});
}
#endif
