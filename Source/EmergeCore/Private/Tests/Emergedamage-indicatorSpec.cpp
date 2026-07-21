// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: damage-direction indicator (shell/combat HUD).
// SHELL stratum: the "you were hit from THERE" indicator. Given the hit source's world bearing and the player's
// facing, computes the relative bearing and the 8-point sector to light up (0=front, clockwise), plus an intensity
// that fades with distance. Standard relative-bearing math; deterministic.
// CONTRACT — NEW class UEmergeDamageIndicator : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeDamageIndicator.h + Private/Shell/EmergeDamageIndicator.cpp, static UFUNCTIONs.
//
//   static float RelativeBearing(float HitDirDeg, float FacingDeg);
//     // wrap (HitDirDeg - FacingDeg) into [0,360): float d = FMath::Fmod(HitDirDeg - FacingDeg, 360.f); if (d < 0.f) d += 360.f; return d;
//   static int32 Sector8(float RelativeBearing);
//     // return ((int32)FMath::RoundToInt(RelativeBearing / 45.0f)) % 8;   // 0 front .. clockwise
//   static float Intensity01(float Distance, float MaxDistance);
//     // closer hit = stronger: return FMath::Clamp(1.0f - Distance / MaxDistance, 0.0f, 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeDamageIndicator.h"

BEGIN_DEFINE_SPEC(FEmergeDamageIndicatorSpec, "Emergence.Shell.DamageIndicator",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDamageIndicatorSpec)
void FEmergeDamageIndicatorSpec::Define()
{
	Describe("RelativeBearing", [this]()
	{
		It("hit bearing minus facing, wrapped to [0,360)", [this]()
		{
			TestEqual(TEXT("hit ahead"), UEmergeDamageIndicator::RelativeBearing(90.f, 90.f), 0.0f);
			TestEqual(TEXT("hit to the right"), UEmergeDamageIndicator::RelativeBearing(90.f, 0.f), 90.0f);
			TestEqual(TEXT("hit to the left wraps"), UEmergeDamageIndicator::RelativeBearing(0.f, 90.f), 270.0f);
			TestEqual(TEXT("over-360 wraps"), UEmergeDamageIndicator::RelativeBearing(450.f, 0.f), 90.0f);
		});
	});

	Describe("Sector8", [this]()
	{
		It("relative bearing to 8-point sector (front, clockwise)", [this]()
		{
			TestEqual(TEXT("front"), UEmergeDamageIndicator::Sector8(0.f), 0);
			TestEqual(TEXT("front-right"), UEmergeDamageIndicator::Sector8(45.f), 1);
			TestEqual(TEXT("right"), UEmergeDamageIndicator::Sector8(90.f), 2);
			TestEqual(TEXT("behind"), UEmergeDamageIndicator::Sector8(180.f), 4);
			TestEqual(TEXT("left"), UEmergeDamageIndicator::Sector8(270.f), 6);
			TestEqual(TEXT("359 wraps to front"), UEmergeDamageIndicator::Sector8(359.f), 0);
		});
	});

	Describe("Intensity01", [this]()
	{
		It("fades with distance, clamped", [this]()
		{
			TestEqual(TEXT("point-blank -> 1"), UEmergeDamageIndicator::Intensity01(0.f, 50.f), 1.0f);
			TestEqual(TEXT("at max -> 0"), UEmergeDamageIndicator::Intensity01(50.f, 50.f), 0.0f);
			TestEqual(TEXT("half -> 0.5"), UEmergeDamageIndicator::Intensity01(25.f, 50.f), 0.5f);
			TestEqual(TEXT("beyond max clamps 0"), UEmergeDamageIndicator::Intensity01(100.f, 50.f), 0.0f);
		});
	});
}
#endif
