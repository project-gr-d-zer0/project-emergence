// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ping marker view-model (shell).
// SHELL stratum: a world ping/marker - it fades over its lifetime, expires, and shrinks with distance so far
// pings don't dominate the screen. Pure view math; deterministic.
// CONTRACT — NEW class UEmergePing : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergePing.h + Private/Shell/EmergePing.cpp, static UFUNCTIONs.
//
//   static float FadeAlpha(float Elapsed, float Lifetime);   // return FMath::Clamp(1.0f - Elapsed / Lifetime, 0.0f, 1.0f);
//   static bool  IsExpired(float Elapsed, float Lifetime);    // return Elapsed >= Lifetime;
//   static float DistanceScale(float Distance, float RefDistance);
//     // if (Distance <= 0.0f) return 1.0f;   return FMath::Clamp(RefDistance / Distance, 0.3f, 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergePing.h"

BEGIN_DEFINE_SPEC(FEmergePingSpec, "Emergence.Shell.Ping",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergePingSpec)
void FEmergePingSpec::Define()
{
	Describe("Fade & expiry", [this]()
	{
		It("fades over lifetime and expires", [this]()
		{
			TestEqual(TEXT("fresh -> 1"), UEmergePing::FadeAlpha(0.f, 5.f), 1.0f);
			TestEqual(TEXT("half -> 0.5"), UEmergePing::FadeAlpha(2.5f, 5.f), 0.5f);
			TestEqual(TEXT("done -> 0"), UEmergePing::FadeAlpha(5.f, 5.f), 0.0f);
			TestTrue(TEXT("expired at lifetime"), UEmergePing::IsExpired(5.f, 5.f));
			TestFalse(TEXT("not expired before"), UEmergePing::IsExpired(4.f, 5.f));
		});
	});

	Describe("DistanceScale", [this]()
	{
		It("shrinks with distance, floored at 0.3", [this]()
		{
			TestEqual(TEXT("at ref -> 1.0"), UEmergePing::DistanceScale(10.f, 10.f), 1.0f);
			TestEqual(TEXT("double distance -> 0.5"), UEmergePing::DistanceScale(20.f, 10.f), 0.5f);
			TestEqual(TEXT("very far floors 0.3"), UEmergePing::DistanceScale(100.f, 10.f), 0.3f);
			TestEqual(TEXT("closer clamps to 1"), UEmergePing::DistanceScale(5.f, 10.f), 1.0f);
		});
	});
}
#endif
