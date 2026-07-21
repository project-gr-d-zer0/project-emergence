// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: trap catch chance (survival).
// Passive food gathering: a set trap has a per-check catch chance by type, doubled when baited. The roll is
// passed in so the test is deterministic (no RNG in asserts). TUNABLE design chances, fixed model.
// CONTRACT — NEW class UEmergeTrap : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeTrap.h + Private/Survival/EmergeTrap.cpp, static UFUNCTIONs.
//   TrapType int32: 0 Snare, 1 Cage, 2 FishTrap.
//
//   static float CatchChancePerCheck(int32 TrapType, bool bBaited);
//     // float base; switch(TrapType){ case 0: base=0.2f; break; case 1: base=0.35f; break; case 2: base=0.3f; break; default: base=0.f; }
//     // return FMath::Clamp(bBaited ? base * 2.0f : base, 0.0f, 1.0f);
//   static bool Caught(float Roll01, float Chance);   // return Roll01 < Chance;
//   static bool IsValidTrap(int32 TrapType);          // return TrapType >= 0 && TrapType <= 2;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeTrap.h"

BEGIN_DEFINE_SPEC(FEmergeTrapSpec, "Emergence.Survival.Trap",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeTrapSpec)
void FEmergeTrapSpec::Define()
{
	Describe("CatchChancePerCheck", [this]()
	{
		It("per-type chance, doubled when baited (clamped)", [this]()
		{
			TestEqual(TEXT("snare unbaited 0.2"), UEmergeTrap::CatchChancePerCheck(0, false), 0.2f);
			TestEqual(TEXT("snare baited 0.4"), UEmergeTrap::CatchChancePerCheck(0, true), 0.4f);
			TestEqual(TEXT("cage unbaited 0.35"), UEmergeTrap::CatchChancePerCheck(1, false), 0.35f);
			TestEqual(TEXT("cage baited 0.7"), UEmergeTrap::CatchChancePerCheck(1, true), 0.7f);
			TestEqual(TEXT("fishtrap baited 0.6"), UEmergeTrap::CatchChancePerCheck(2, true), 0.6f);
			TestEqual(TEXT("invalid -> 0"), UEmergeTrap::CatchChancePerCheck(9, true), 0.0f);
		});
	});

	Describe("Caught & IsValidTrap", [this]()
	{
		It("catch roll and trap validity", [this]()
		{
			TestTrue(TEXT("roll under chance catches"), UEmergeTrap::Caught(0.1f, 0.2f));
			TestFalse(TEXT("roll over chance misses"), UEmergeTrap::Caught(0.3f, 0.2f));
			TestTrue(TEXT("snare valid"), UEmergeTrap::IsValidTrap(0));
			TestFalse(TEXT("type 5 invalid"), UEmergeTrap::IsValidTrap(5));
		});
	});
}
#endif
