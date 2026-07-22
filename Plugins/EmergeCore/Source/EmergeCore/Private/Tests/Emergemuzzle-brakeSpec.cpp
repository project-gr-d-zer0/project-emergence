// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: muzzle brake (combat/gunplay).
// A muzzle brake cuts recoil (feeds EmergeRecoil/EmergeMuzzleClimb) at the cost of a louder, wider side-blast
// that gives your position away more (EmergeNoiseEmission). TUNABLE design factors, fixed deterministic model.
// CONTRACT — NEW class UEmergeMuzzleBrake : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeMuzzleBrake.h + Private/Combat/EmergeMuzzleBrake.cpp, static UFUNCTIONs.
//
//   static float RecoilMultiplier(bool bBrake);        // return bBrake ? 0.75f : 1.0f;
//   static float SideBlastMultiplier(bool bBrake);      // return bBrake ? 1.5f : 1.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeMuzzleBrake.h"

BEGIN_DEFINE_SPEC(FEmergeMuzzleBrakeSpec, "Emergence.Combat.MuzzleBrake",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMuzzleBrakeSpec)
void FEmergeMuzzleBrakeSpec::Define()
{
	Describe("Muzzle brake tradeoff", [this]()
	{
		It("less recoil, more side blast", [this]()
		{
			TestEqual(TEXT("brake cuts recoil to 0.75"), UEmergeMuzzleBrake::RecoilMultiplier(true), 0.75f);
			TestEqual(TEXT("no brake full recoil"), UEmergeMuzzleBrake::RecoilMultiplier(false), 1.0f);
			TestEqual(TEXT("brake 1.5x side blast"), UEmergeMuzzleBrake::SideBlastMultiplier(true), 1.5f);
			TestEqual(TEXT("no brake normal blast"), UEmergeMuzzleBrake::SideBlastMultiplier(false), 1.0f);
		});
	});
}
#endif
