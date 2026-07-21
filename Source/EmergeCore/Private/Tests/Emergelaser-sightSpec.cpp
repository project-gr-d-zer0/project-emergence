// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: laser sight (combat/gunplay).
// A laser tightens hip-fire spread (feeding EmergeSpreadStance) but the visible dot can give your position away
// to a facing enemy within range. TUNABLE design factors, fixed deterministic model.
// CONTRACT — NEW class UEmergeLaserSight : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeLaserSight.h + Private/Combat/EmergeLaserSight.cpp, static UFUNCTIONs.
//
//   static float HipfireSpreadMultiplier(bool bLaserOn);   // return bLaserOn ? 0.6f : 1.0f;
//   static bool  IsVisibleToEnemy(float Distance, float MaxVisRange, bool bLaserOn);
//     // return bLaserOn && (Distance <= MaxVisRange);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeLaserSight.h"

BEGIN_DEFINE_SPEC(FEmergeLaserSightSpec, "Emergence.Combat.LaserSight",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeLaserSightSpec)
void FEmergeLaserSightSpec::Define()
{
	Describe("Laser effects", [this]()
	{
		It("tightens hipfire but reveals you", [this]()
		{
			TestEqual(TEXT("laser on tightens to 0.6"), UEmergeLaserSight::HipfireSpreadMultiplier(true), 0.6f);
			TestEqual(TEXT("laser off full spread"), UEmergeLaserSight::HipfireSpreadMultiplier(false), 1.0f);
			TestTrue(TEXT("dot visible up close & on"), UEmergeLaserSight::IsVisibleToEnemy(20.f, 50.f, true));
			TestFalse(TEXT("out of range not visible"), UEmergeLaserSight::IsVisibleToEnemy(60.f, 50.f, true));
			TestFalse(TEXT("laser off never visible"), UEmergeLaserSight::IsVisibleToEnemy(20.f, 50.f, false));
		});
	});
}
#endif
