// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: L4D-style director intensity math
// (research: Booth AIIDE-09 — intensity from damage + nearby deaths; decay FROZEN while engaged).
// CONTRACT — NEW class UEmergeDirectorIntensity : public UBlueprintFunctionLibrary
//   in Public/AI/EmergeDirectorIntensity.h + Private/AI/EmergeDirectorIntensity.cpp, static UFUNCTIONs:
//   static float IntensityFromDamage(float DamageFraction);   // = FMath::Clamp(DamageFraction, 0.f, 1.f)
//   static float IntensityFromNearbyDeath(float DistUu);
//     // = 0.3f * FMath::Clamp(300.f / FMath::Max(DistUu, 300.f), 0.f, 1.f)
//   static float Decay(float Current, float Dt, bool bEnemiesEngaged);
//     // engaged -> Current unchanged; else half-life 10s: Current * FMath::Pow(0.5f, Dt / 10.f)
// Deterministic, exact golden values.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "AI/EmergeDirectorIntensity.h"

BEGIN_DEFINE_SPEC(FEmergeDirectorIntensitySpec, "Emergence.AI.DirectorIntensity",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDirectorIntensitySpec)
void FEmergeDirectorIntensitySpec::Define()
{
	Describe("event intensity", [this]()
	{
		It("damage fraction clamps to 0..1", [this]()
		{
			TestEqual(TEXT("0.4"), UEmergeDirectorIntensity::IntensityFromDamage(0.4f), 0.4f);
			TestEqual(TEXT("1.7 -> 1"), UEmergeDirectorIntensity::IntensityFromDamage(1.7f), 1.f);
			TestEqual(TEXT("-0.2 -> 0"), UEmergeDirectorIntensity::IntensityFromDamage(-0.2f), 0.f);
		});
		It("nearby death is inverse distance capped at 0.3", [this]()
		{
			TestNearlyEqual(TEXT("100uu -> 0.3"), UEmergeDirectorIntensity::IntensityFromNearbyDeath(100.f), 0.3f, 1e-4f);
			TestNearlyEqual(TEXT("300uu -> 0.3"), UEmergeDirectorIntensity::IntensityFromNearbyDeath(300.f), 0.3f, 1e-4f);
			TestNearlyEqual(TEXT("600uu -> 0.15"), UEmergeDirectorIntensity::IntensityFromNearbyDeath(600.f), 0.15f, 1e-4f);
			TestNearlyEqual(TEXT("1200uu -> 0.075"), UEmergeDirectorIntensity::IntensityFromNearbyDeath(1200.f), 0.075f, 1e-4f);
		});
	});
	Describe("Decay", [this]()
	{
		It("halves per 10s when not engaged", [this]()
		{
			TestNearlyEqual(TEXT("1.0 @10s -> 0.5"), UEmergeDirectorIntensity::Decay(1.f, 10.f, false), 0.5f, 1e-4f);
			TestNearlyEqual(TEXT("0.8 @20s -> 0.2"), UEmergeDirectorIntensity::Decay(0.8f, 20.f, false), 0.2f, 1e-4f);
		});
		It("freezes while enemies engage", [this]()
		{
			TestEqual(TEXT("0.6 engaged"), UEmergeDirectorIntensity::Decay(0.6f, 5.f, true), 0.6f);
		});
	});
}
#endif
