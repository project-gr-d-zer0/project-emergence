// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: sight zeroing / holdover (combat/gunplay).
// A sight is zeroed at a set distance; past it the round has dropped so you must hold OVER, closer you hold
// under. Composes with EmergeBulletDrop. Standard marksmanship math; deterministic.
// CONTRACT — NEW class UEmergeSightZero : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeSightZero.h + Private/Combat/EmergeSightZero.cpp, static UFUNCTIONs.
//
//   static float HoldoverAtRange(float Distance, float ZeroDistance, float DropPerMeter);
//     // return (Distance - ZeroDistance) * DropPerMeter;   // + = hold over, - = hold under, 0 at zero
//   static bool  IsZeroedFor(float Distance, float ZeroDistance, float Tolerance);
//     // return FMath::Abs(Distance - ZeroDistance) <= Tolerance;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeSightZero.h"

BEGIN_DEFINE_SPEC(FEmergeSightZeroSpec, "Emergence.Combat.SightZero",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSightZeroSpec)
void FEmergeSightZeroSpec::Define()
{
	Describe("Holdover", [this]()
	{
		It("zero at zero-distance, hold over past, under closer", [this]()
		{
			TestEqual(TEXT("at zero -> 0"), UEmergeSightZero::HoldoverAtRange(100.f, 100.f, 0.01f), 0.0f);
			TestEqual(TEXT("past zero -> hold over"), UEmergeSightZero::HoldoverAtRange(200.f, 100.f, 0.01f), 1.0f);
			TestEqual(TEXT("closer -> hold under"), UEmergeSightZero::HoldoverAtRange(50.f, 100.f, 0.01f), -0.5f);
		});
	});

	Describe("IsZeroedFor", [this]()
	{
		It("within tolerance counts as zeroed", [this]()
		{
			TestTrue(TEXT("exact"), UEmergeSightZero::IsZeroedFor(100.f, 100.f, 10.f));
			TestTrue(TEXT("within tolerance"), UEmergeSightZero::IsZeroedFor(105.f, 100.f, 10.f));
			TestFalse(TEXT("outside tolerance"), UEmergeSightZero::IsZeroedFor(120.f, 100.f, 10.f));
		});
	});
}
#endif
