// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fire-mode cycle (combat/gunplay).
// The selector-switch model: cycle through the weapon's available fire modes (safe/semi/burst/auto), report how
// many rounds a single trigger pull fires, and whether the weapon can fire at all. Deterministic state logic.
// CONTRACT — NEW class UEmergeFireMode : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeFireMode.h + Private/Combat/EmergeFireMode.cpp, static UFUNCTIONs.
//   Mode int32: 0 Safe, 1 Semi, 2 Burst, 3 Auto.
//
//   static int32 NextMode(int32 Current, int32 MaxMode);   // return (Current + 1) % (MaxMode + 1);
//   static int32 RoundsPerPull(int32 Mode);
//     // switch: 0 -> 0; 1 -> 1; 2 -> 3; 3 -> -1 (continuous); default -> 0;
//   static bool  CanFire(int32 Mode);   // return Mode != 0;   // not on safe
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeFireMode.h"

BEGIN_DEFINE_SPEC(FEmergeFireModeSpec, "Emergence.Combat.FireMode",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFireModeSpec)
void FEmergeFireModeSpec::Define()
{
	Describe("NextMode", [this]()
	{
		It("cycles through available modes, wrapping", [this]()
		{
			TestEqual(TEXT("safe -> semi"), UEmergeFireMode::NextMode(0, 3), 1);
			TestEqual(TEXT("semi -> burst"), UEmergeFireMode::NextMode(1, 3), 2);
			TestEqual(TEXT("auto wraps to safe"), UEmergeFireMode::NextMode(3, 3), 0);
			TestEqual(TEXT("2-mode weapon wraps"), UEmergeFireMode::NextMode(1, 1), 0);
		});
	});

	Describe("RoundsPerPull & CanFire", [this]()
	{
		It("per-mode rounds and the safe gate", [this]()
		{
			TestEqual(TEXT("safe 0"), UEmergeFireMode::RoundsPerPull(0), 0);
			TestEqual(TEXT("semi 1"), UEmergeFireMode::RoundsPerPull(1), 1);
			TestEqual(TEXT("burst 3"), UEmergeFireMode::RoundsPerPull(2), 3);
			TestEqual(TEXT("auto continuous -1"), UEmergeFireMode::RoundsPerPull(3), -1);
			TestFalse(TEXT("safe cannot fire"), UEmergeFireMode::CanFire(0));
			TestTrue(TEXT("semi can fire"), UEmergeFireMode::CanFire(1));
			TestTrue(TEXT("auto can fire"), UEmergeFireMode::CanFire(3));
		});
	});
}
#endif
