// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fall damage curve (DayZ-style).
// A fall's severity scales with height: below a safe threshold nothing happens; between the low and high
// thresholds damage ramps linearly to 100%; above the high threshold it is capped. Health, shock and
// broken-legs each have their own height band. The returned FRACTION multiplies the downstream max
// health/shock damage. Numbers exact from decompiled DayZ dayzplayerimplementfalldamage.c: health 5m..14m,
// shock 3m..12m, broken legs 5m..9m; anything under 3m is a safe fall.
// CONTRACT — NEW class UEmergeFallDamage : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeFallDamage.h + Private/Combat/EmergeFallDamage.cpp, static UFUNCTIONs:
//
//   static float DamageFraction(float HeightM, float LowM, float HighM);
//     // Linear ramp 0..1 across [LowM, HighM], clamped. Guard HighM<=LowM (return 0/1 at the boundary).
//     // if (HeightM <= LowM) return 0.0f;
//     // if (HeightM >= HighM) return 1.0f;
//     // return (HeightM - LowM) / (HighM - LowM);
//
//   static float HealthDamageFraction(float HeightM);   // return DamageFraction(HeightM, 5.0f, 14.0f);
//   static float ShockDamageFraction(float HeightM);    // return DamageFraction(HeightM, 3.0f, 12.0f);
//
//   static bool CanBreakLegs(float HeightM);            // return HeightM >= 5.0f;   // broken-legs low threshold
//   static bool IsSafeFall(float HeightM);             // return HeightM < 3.0f;    // below all fall effects
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeFallDamage.h"

BEGIN_DEFINE_SPEC(FEmergeFallDamageCurveSpec, "Emergence.Combat.FallDamage",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFallDamageCurveSpec)
void FEmergeFallDamageCurveSpec::Define()
{
	Describe("DamageFraction", [this]()
	{
		It("linear ramp across the band, clamped at both ends", [this]()
		{
			TestEqual(TEXT("below low -> 0"), UEmergeFallDamage::DamageFraction(4.f, 5.f, 14.f), 0.0f);
			TestEqual(TEXT("at low -> 0"), UEmergeFallDamage::DamageFraction(5.f, 5.f, 14.f), 0.0f);
			TestEqual(TEXT("at high -> 1"), UEmergeFallDamage::DamageFraction(14.f, 5.f, 14.f), 1.0f);
			TestEqual(TEXT("above high -> 1"), UEmergeFallDamage::DamageFraction(20.f, 5.f, 14.f), 1.0f);
			TestEqual(TEXT("midpoint 9.5 -> 0.5"), UEmergeFallDamage::DamageFraction(9.5f, 5.f, 14.f), 0.5f);
		});
	});

	Describe("Health & shock bands (exact DayZ thresholds)", [this]()
	{
		It("health ramps 5..14m", [this]()
		{
			TestEqual(TEXT("4m -> 0"), UEmergeFallDamage::HealthDamageFraction(4.f), 0.0f);
			TestEqual(TEXT("14m -> 1"), UEmergeFallDamage::HealthDamageFraction(14.f), 1.0f);
			TestEqual(TEXT("9.5m -> 0.5"), UEmergeFallDamage::HealthDamageFraction(9.5f), 0.5f);
		});
		It("shock ramps 3..12m", [this]()
		{
			TestEqual(TEXT("3m -> 0"), UEmergeFallDamage::ShockDamageFraction(3.f), 0.0f);
			TestEqual(TEXT("12m -> 1"), UEmergeFallDamage::ShockDamageFraction(12.f), 1.0f);
			TestEqual(TEXT("7.5m -> 0.5"), UEmergeFallDamage::ShockDamageFraction(7.5f), 0.5f);
		});
	});

	Describe("Broken legs + safe fall", [this]()
	{
		It("thresholds", [this]()
		{
			TestFalse(TEXT("4m no legs"), UEmergeFallDamage::CanBreakLegs(4.f));
			TestTrue(TEXT("5m can break legs"), UEmergeFallDamage::CanBreakLegs(5.f));
			TestTrue(TEXT("2.9m safe"), UEmergeFallDamage::IsSafeFall(2.9f));
			TestFalse(TEXT("3m not safe"), UEmergeFallDamage::IsSafeFall(3.f));
		});
	});
}
#endif
