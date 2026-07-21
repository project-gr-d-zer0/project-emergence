// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: health vignette view-model (shell).
// SHELL stratum: the low-health screen feedback - a red vignette that grows as health falls, desaturation, and a
// heartbeat pulse near death (the DayZ/Tarkov "you're dying" screen). Pairs with EmergeVitalHud. Pure view math.
// CONTRACT — NEW class UEmergeHealthVignette : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeHealthVignette.h + Private/Shell/EmergeHealthVignette.cpp, static UFUNCTIONs.
//
//   static float VignetteIntensity(float Health01);
//     // grows below 0.3: return FMath::Clamp((0.3f - Health01) / 0.3f, 0.0f, 1.0f);
//   static float Desaturation01(float Health01);
//     // grows below 0.5: return FMath::Clamp((0.5f - Health01) / 0.5f, 0.0f, 1.0f);
//   static bool  IsCriticalPulse(float Health01);   // return Health01 < 0.1f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeHealthVignette.h"

BEGIN_DEFINE_SPEC(FEmergeHealthVignetteSpec, "Emergence.Shell.HealthVignette",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHealthVignetteSpec)
void FEmergeHealthVignetteSpec::Define()
{
	Describe("Vignette & desaturation", [this]()
	{
		It("intensifies as health falls", [this]()
		{
			TestEqual(TEXT("full health no vignette"), UEmergeHealthVignette::VignetteIntensity(1.0f), 0.0f);
			TestEqual(TEXT("at 0.3 threshold -> 0"), UEmergeHealthVignette::VignetteIntensity(0.3f), 0.0f);
			TestEqual(TEXT("0.15 -> 0.5"), UEmergeHealthVignette::VignetteIntensity(0.15f), 0.5f);
			TestEqual(TEXT("dead -> full"), UEmergeHealthVignette::VignetteIntensity(0.0f), 1.0f);
			TestEqual(TEXT("full health no desat"), UEmergeHealthVignette::Desaturation01(0.5f), 0.0f);
			TestEqual(TEXT("0.25 -> 0.5 desat"), UEmergeHealthVignette::Desaturation01(0.25f), 0.5f);
			TestEqual(TEXT("dead -> full desat"), UEmergeHealthVignette::Desaturation01(0.0f), 1.0f);
		});
	});

	Describe("IsCriticalPulse", [this]()
	{
		It("heartbeat pulse under 10% health", [this]()
		{
			TestTrue(TEXT("0.05 pulses"), UEmergeHealthVignette::IsCriticalPulse(0.05f));
			TestFalse(TEXT("0.1 no pulse"), UEmergeHealthVignette::IsCriticalPulse(0.1f));
			TestFalse(TEXT("healthy no pulse"), UEmergeHealthVignette::IsCriticalPulse(0.5f));
		});
	});
}
#endif
