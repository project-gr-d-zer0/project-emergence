// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: bullet drop arc (combat/gunplay).
// Real projectile ballistics: a round falls under gravity over its flight time (t = distance / muzzle velocity;
// drop = 1/2 g t^2), so long shots need holdover. Composes with EmergeBallisticDropoff. Standard physics; exact.
// CONTRACT — NEW class UEmergeBulletDrop : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeBulletDrop.h + Private/Combat/EmergeBulletDrop.cpp, static UFUNCTIONs.
//
//   static float FlightTime(float Distance, float Velocity);   // return Distance / Velocity;
//   static float Drop(float Distance, float Velocity, float Gravity);
//     // const float t = Distance / Velocity;   return 0.5f * Gravity * t * t;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeBulletDrop.h"

BEGIN_DEFINE_SPEC(FEmergeBulletDropSpec, "Emergence.Combat.BulletDrop",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBulletDropSpec)
void FEmergeBulletDropSpec::Define()
{
	Describe("FlightTime & Drop", [this]()
	{
		It("t = d/v; drop = 1/2 g t^2", [this]()
		{
			TestEqual(TEXT("900m @900m/s -> 1s"), UEmergeBulletDrop::FlightTime(900.f, 900.f), 1.0f);
			TestEqual(TEXT("1800m @900m/s -> 2s"), UEmergeBulletDrop::FlightTime(1800.f, 900.f), 2.0f);
			TestEqual(TEXT("drop at 1s -> 4.9"), UEmergeBulletDrop::Drop(900.f, 900.f, 9.8f), 4.9f);
			TestEqual(TEXT("drop at 2s -> 19.6"), UEmergeBulletDrop::Drop(1800.f, 900.f, 9.8f), 19.6f);
			TestEqual(TEXT("no distance -> no drop"), UEmergeBulletDrop::Drop(0.f, 900.f, 9.8f), 0.0f);
		});
	});
}
#endif
