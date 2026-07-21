// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fracture / splint (DayZ-style).
// A broken limb hobbles movement and blocks sprinting until a splint stabilizes it; a splinted fracture still
// heals (slowly) over time. Composes with EmergeFallDamage (broken-legs outcome) and EmergeStaminaModel.
// TUNABLE design values, fixed deterministic model.
// CONTRACT — NEW class UEmergeFracture : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeFracture.h + Private/Survival/EmergeFracture.cpp, static UFUNCTIONs.
//
//   static float MoveSpeedMultiplier(bool bFractured, bool bSplinted);
//     // if (!bFractured) return 1.0f;   return bSplinted ? 0.7f : 0.3f;
//   static bool  CanSprint(bool bFractured, bool bSplinted);   // return (!bFractured) || bSplinted;
//   static float FractureHealPerTick(bool bSplinted, float DeltaT);
//     // return (bSplinted ? 0.02f : 0.005f) * DeltaT;   // splinted mends faster
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeFracture.h"

BEGIN_DEFINE_SPEC(FEmergeFractureSpec, "Emergence.Survival.Fracture",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFractureSpec)
void FEmergeFractureSpec::Define()
{
	Describe("Mobility", [this]()
	{
		It("hobble unsplinted, limp splinted, normal when healthy; sprint gated", [this]()
		{
			TestEqual(TEXT("healthy full speed"), UEmergeFracture::MoveSpeedMultiplier(false, false), 1.0f);
			TestEqual(TEXT("fractured unsplinted 0.3"), UEmergeFracture::MoveSpeedMultiplier(true, false), 0.3f);
			TestEqual(TEXT("fractured splinted 0.7"), UEmergeFracture::MoveSpeedMultiplier(true, true), 0.7f);
			TestTrue(TEXT("healthy can sprint"), UEmergeFracture::CanSprint(false, false));
			TestFalse(TEXT("unsplinted fracture cannot sprint"), UEmergeFracture::CanSprint(true, false));
			TestTrue(TEXT("splinted fracture can sprint"), UEmergeFracture::CanSprint(true, true));
		});
	});

	Describe("Healing", [this]()
	{
		It("splint mends faster", [this]()
		{
			TestEqual(TEXT("splinted 0.02/s"), UEmergeFracture::FractureHealPerTick(true, 1.0f), 0.02f);
			TestEqual(TEXT("unsplinted 0.005/s"), UEmergeFracture::FractureHealPerTick(false, 1.0f), 0.005f);
			TestEqual(TEXT("splinted dt=2 -> 0.04"), UEmergeFracture::FractureHealPerTick(true, 2.0f), 0.04f);
		});
	});
}
#endif
