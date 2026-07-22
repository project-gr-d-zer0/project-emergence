// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: suppressor (combat/gunplay).
// A suppressor cuts the gunshot's noise radius (feeding EmergeNoiseEmission / AI hearing) and its muzzle flash.
// TUNABLE reduction factors, fixed deterministic model.
// CONTRACT — NEW class UEmergeSuppressor : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeSuppressor.h + Private/Combat/EmergeSuppressor.cpp, static UFUNCTIONs.
//
//   static float NoiseRadius(float BaseRadius, bool bSuppressed);   // return bSuppressed ? BaseRadius * 0.35f : BaseRadius;
//   static float MuzzleFlashScale(bool bSuppressed);                 // return bSuppressed ? 0.2f : 1.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeSuppressor.h"

BEGIN_DEFINE_SPEC(FEmergeSuppressorSpec, "Emergence.Combat.Suppressor",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSuppressorSpec)
void FEmergeSuppressorSpec::Define()
{
	Describe("Suppressor effects", [this]()
	{
		It("cuts noise radius and muzzle flash", [this]()
		{
			TestEqual(TEXT("suppressed noise 35%"), UEmergeSuppressor::NoiseRadius(100.f, true), 35.0f);
			TestEqual(TEXT("unsuppressed full noise"), UEmergeSuppressor::NoiseRadius(100.f, false), 100.0f);
			TestEqual(TEXT("suppressed flash 0.2"), UEmergeSuppressor::MuzzleFlashScale(true), 0.2f);
			TestEqual(TEXT("unsuppressed flash 1.0"), UEmergeSuppressor::MuzzleFlashScale(false), 1.0f);
		});
	});
}
#endif
