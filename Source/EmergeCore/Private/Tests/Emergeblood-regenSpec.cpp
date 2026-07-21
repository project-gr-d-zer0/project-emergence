// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: blood regeneration (DayZ-style survival).
// Blood slowly regenerates, gated by hydration and energy (starve/dehydrate -> no regen); doubles while
// unconscious+critical; saline IV gives a flat independent boost. Numbers are exact from decompiled DayZ
// PlayerConstants: base 0.3/s, stat modifiers LOW=0/MID=0.65/HIGH=1.0, unconscious x2 at blood<=3000,
// saline +3/s, fatal at blood<=2500. Composes with EmergeVitals/Medical + water/energy needs.
// CONTRACT — NEW class UEmergeBloodRegen : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeBloodRegen.h + Private/Survival/EmergeBloodRegen.cpp, static UFUNCTIONs:
//
//   static float RegenStatModifier(int32 StatLevel);
//     // Water/energy stat level gate: 0=LOW->0.0, 1=MID->0.65, 2=HIGH->1.0; any other -> 1.0 (treat as high).
//
//   static float BloodRegenPerSec(int32 WaterLevel, int32 EnergyLevel, bool bUnconscious, float CurrentBlood);
//     // float r = 0.3f * RegenStatModifier(WaterLevel) * RegenStatModifier(EnergyLevel);
//     // if (bUnconscious && CurrentBlood <= 3000.f) r *= 2.0f;
//     // return r;
//
//   static float SalineBoostPerSec();
//     // return 3.0f;   // flat blood regen from a saline IV, independent of the gated base rate
//
//   static bool IsFatalBlood(float CurrentBlood);
//     // return CurrentBlood <= 2500.f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeBloodRegen.h"

BEGIN_DEFINE_SPEC(FEmergeBloodRegenSpec, "Emergence.Survival.BloodRegen",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBloodRegenSpec)
void FEmergeBloodRegenSpec::Define()
{
	Describe("RegenStatModifier", [this]()
	{
		It("gates regen by hydration/energy stat level (exact DayZ values)", [this]()
		{
			TestEqual(TEXT("LOW -> 0"), UEmergeBloodRegen::RegenStatModifier(0), 0.0f);
			TestEqual(TEXT("MID -> 0.65"), UEmergeBloodRegen::RegenStatModifier(1), 0.65f);
			TestEqual(TEXT("HIGH -> 1.0"), UEmergeBloodRegen::RegenStatModifier(2), 1.0f);
			TestEqual(TEXT("unknown -> 1.0"), UEmergeBloodRegen::RegenStatModifier(9), 1.0f);
		});
	});

	Describe("BloodRegenPerSec", [this]()
	{
		It("multiplies base by both gates; dehydration or starvation halts regen", [this]()
		{
			TestEqual(TEXT("full water+energy -> 0.3"), UEmergeBloodRegen::BloodRegenPerSec(2, 2, false, 4000.f), 0.3f);
			TestEqual(TEXT("mid water -> 0.195"), UEmergeBloodRegen::BloodRegenPerSec(1, 2, false, 4000.f), 0.195f);
			TestEqual(TEXT("dehydrated -> 0"), UEmergeBloodRegen::BloodRegenPerSec(0, 2, false, 4000.f), 0.0f);
			TestEqual(TEXT("mid+mid -> 0.12675"), UEmergeBloodRegen::BloodRegenPerSec(1, 1, false, 4000.f), 0.12675f);
		});
		It("doubles while unconscious AND blood critical (<=3000)", [this]()
		{
			TestEqual(TEXT("uncon+critical -> 0.6"), UEmergeBloodRegen::BloodRegenPerSec(2, 2, true, 2900.f), 0.6f);
			TestEqual(TEXT("uncon but blood>3000 -> 0.3"), UEmergeBloodRegen::BloodRegenPerSec(2, 2, true, 3500.f), 0.3f);
			TestEqual(TEXT("uncon at exactly 3000 -> 0.6"), UEmergeBloodRegen::BloodRegenPerSec(2, 2, true, 3000.f), 0.6f);
		});
	});

	Describe("Saline + fatal", [this]()
	{
		It("saline flat boost and fatal threshold", [this]()
		{
			TestEqual(TEXT("saline 3/s"), UEmergeBloodRegen::SalineBoostPerSec(), 3.0f);
			TestTrue(TEXT("2500 fatal"), UEmergeBloodRegen::IsFatalBlood(2500.f));
			TestTrue(TEXT("2000 fatal"), UEmergeBloodRegen::IsFatalBlood(2000.f));
			TestFalse(TEXT("2501 not fatal"), UEmergeBloodRegen::IsFatalBlood(2501.f));
		});
	});
}
#endif
