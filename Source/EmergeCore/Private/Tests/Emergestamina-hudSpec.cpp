// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: stamina HUD view-model (DayZ-style).
// SHELL stratum: the display + affordance layer over the existing EmergeStaminaModel sim - it drives the stamina
// bar fill and decides which traversal/melee prompts are enabled (can I jump / vault / climb / heavy-hit right
// now?). Pure view logic, no UMG. Numbers exact from decompiled DayZ constants.c StaminaHandlerCfg: STAMINA_MAX
// 100, action thresholds JUMP 25 / VAULT 20 / CLIMB 42 / MELEE_HEAVY 25 / ROLL 5, regen cooldowns DEPLETION 0.45
// / EXHAUSTION 0.5. Does NOT recompute max-stamina (that is EmergeStaminaModel's job) - it takes current+max.
// CONTRACT — NEW class UEmergeStaminaHud : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeStaminaHud.h + Private/Shell/EmergeStaminaHud.cpp, static UFUNCTIONs.
//
//   static float BarFill01(float Current, float MaxStamina);   // return FMath::Clamp(Current / MaxStamina, 0.f, 1.f);
//   static bool  CanPerform(float Current, float Threshold);   // return Current >= Threshold; // affordance gate
//   static float JumpThreshold();        // return 25.0f;   // STAMINA_JUMP_THRESHOLD
//   static float VaultThreshold();       // return 20.0f;   // STAMINA_VAULT_THRESHOLD
//   static float ClimbThreshold();       // return 42.0f;   // STAMINA_CLIMB_THRESHOLD
//   static float HeavyMeleeThreshold();  // return 25.0f;   // STAMINA_MELEE_HEAVY_THRESHOLD
//   static float RollThreshold();        // return 5.0f;    // STAMINA_ROLL_THRESHOLD
//   static float RegenCooldownSec(bool bExhausted);  // return bExhausted ? 0.5f : 0.45f; // EXHAUSTION vs DEPLETION
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeStaminaHud.h"

BEGIN_DEFINE_SPEC(FEmergeStaminaHudSpec, "Emergence.Shell.StaminaHud",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaminaHudSpec)
void FEmergeStaminaHudSpec::Define()
{
	Describe("BarFill01", [this]()
	{
		It("current/max clamped to [0,1]", [this]()
		{
			TestEqual(TEXT("half of 100"), UEmergeStaminaHud::BarFill01(50.f, 100.f), 0.5f);
			TestEqual(TEXT("full"), UEmergeStaminaHud::BarFill01(100.f, 100.f), 1.0f);
			TestEqual(TEXT("empty"), UEmergeStaminaHud::BarFill01(0.f, 100.f), 0.0f);
			TestEqual(TEXT("over-max (load-reduced bar) clamps"), UEmergeStaminaHud::BarFill01(75.f, 50.f), 1.0f);
		});
	});

	Describe("Action affordance thresholds (exact DayZ units)", [this]()
	{
		It("exposes the traversal/melee thresholds", [this]()
		{
			TestEqual(TEXT("jump 25"), UEmergeStaminaHud::JumpThreshold(), 25.0f);
			TestEqual(TEXT("vault 20"), UEmergeStaminaHud::VaultThreshold(), 20.0f);
			TestEqual(TEXT("climb 42"), UEmergeStaminaHud::ClimbThreshold(), 42.0f);
			TestEqual(TEXT("heavy melee 25"), UEmergeStaminaHud::HeavyMeleeThreshold(), 25.0f);
			TestEqual(TEXT("roll 5"), UEmergeStaminaHud::RollThreshold(), 5.0f);
		});
		It("gates an action on having enough stamina (boundary inclusive)", [this]()
		{
			TestTrue(TEXT("30 can jump (>=25)"), UEmergeStaminaHud::CanPerform(30.f, UEmergeStaminaHud::JumpThreshold()));
			TestTrue(TEXT("exactly 25 can jump"), UEmergeStaminaHud::CanPerform(25.f, UEmergeStaminaHud::JumpThreshold()));
			TestFalse(TEXT("24 cannot jump"), UEmergeStaminaHud::CanPerform(24.f, UEmergeStaminaHud::JumpThreshold()));
			TestFalse(TEXT("30 cannot climb (<42)"), UEmergeStaminaHud::CanPerform(30.f, UEmergeStaminaHud::ClimbThreshold()));
			TestTrue(TEXT("5 can roll"), UEmergeStaminaHud::CanPerform(5.f, UEmergeStaminaHud::RollThreshold()));
		});
	});

	Describe("Regen cooldown", [this]()
	{
		It("depletion 0.45s, exhaustion 0.5s", [this]()
		{
			TestEqual(TEXT("depletion cooldown"), UEmergeStaminaHud::RegenCooldownSec(false), 0.45f);
			TestEqual(TEXT("exhaustion cooldown"), UEmergeStaminaHud::RegenCooldownSec(true), 0.5f);
		});
	});
}
#endif
