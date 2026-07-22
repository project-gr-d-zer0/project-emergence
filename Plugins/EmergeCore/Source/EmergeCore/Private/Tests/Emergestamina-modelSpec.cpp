// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: stamina economy (DayZ-style).
// Load weight shrinks the max stamina bar (encumbrance); actions drain it (sprint/jump/climb/melee) and
// it regenerates by movement state. Feeds the existing EmergeStaminaComponent as the corpus data model.
// Numbers exact from decompiled DayZ constants.c: max 100, load penalty 1.75 units/kg floored at 5;
// sprint drain 4/1/3 (stand/crouch/prone) per sec; jump 25 / vault 20 / climb 42 / roll 5 / melee light 5
// heavy 25 / evade 8; regen idle 5 / walk 4 / jog 2 / swim 1 / ladder 1 / roll 0 per sec.
// CONTRACT — NEW class UEmergeStaminaModel : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeStaminaModel.h + Private/Survival/EmergeStaminaModel.cpp, static UFUNCTIONs:
//
//   static float MaxStaminaForLoad(float LoadKg);
//     // return FMath::Max(5.0f, 100.0f - 1.75f * LoadKg);
//
//   static float SprintDrainPerSec(int32 Stance);
//     // Stance: 0=Standing->4.0, 1=Crouched->1.0, 2=Prone->3.0; any other -> 4.0.
//
//   static float ActionDrain(int32 Action);
//     // One-shot drains. Action: 0=Jump->25.0, 1=Vault->20.0, 2=Climb->42.0, 3=Roll->5.0,
//     // 4=MeleeLight->5.0, 5=MeleeHeavy->25.0, 6=Evade->8.0; any other -> 0.0.
//
//   static float RegenPerSec(int32 MoveState);
//     // MoveState: 0=Idle->5.0, 1=Walk->4.0, 2=Jog->2.0, 3=Swim->1.0, 4=Ladder->1.0, 5=Roll->0.0;
//     // any other -> 0.0.
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeStaminaModel.h"

BEGIN_DEFINE_SPEC(FEmergeStaminaModelSpec, "Emergence.Survival.StaminaModel",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaminaModelSpec)
void FEmergeStaminaModelSpec::Define()
{
	Describe("MaxStaminaForLoad", [this]()
	{
		It("load shrinks the bar 1.75/kg, floored at 5", [this]()
		{
			TestEqual(TEXT("unloaded -> 100"), UEmergeStaminaModel::MaxStaminaForLoad(0.f), 100.0f);
			TestEqual(TEXT("20kg -> 65"), UEmergeStaminaModel::MaxStaminaForLoad(20.f), 65.0f);
			TestEqual(TEXT("40kg -> 30"), UEmergeStaminaModel::MaxStaminaForLoad(40.f), 30.0f);
			TestEqual(TEXT("overloaded 100kg -> floor 5"), UEmergeStaminaModel::MaxStaminaForLoad(100.f), 5.0f);
		});
	});

	Describe("SprintDrainPerSec", [this]()
	{
		It("stance-dependent sprint drain", [this]()
		{
			TestEqual(TEXT("standing -> 4"), UEmergeStaminaModel::SprintDrainPerSec(0), 4.0f);
			TestEqual(TEXT("crouched -> 1"), UEmergeStaminaModel::SprintDrainPerSec(1), 1.0f);
			TestEqual(TEXT("prone -> 3"), UEmergeStaminaModel::SprintDrainPerSec(2), 3.0f);
			TestEqual(TEXT("unknown -> 4"), UEmergeStaminaModel::SprintDrainPerSec(9), 4.0f);
		});
	});

	Describe("ActionDrain", [this]()
	{
		It("one-shot action drains (exact DayZ values)", [this]()
		{
			TestEqual(TEXT("jump"), UEmergeStaminaModel::ActionDrain(0), 25.0f);
			TestEqual(TEXT("vault"), UEmergeStaminaModel::ActionDrain(1), 20.0f);
			TestEqual(TEXT("climb"), UEmergeStaminaModel::ActionDrain(2), 42.0f);
			TestEqual(TEXT("roll"), UEmergeStaminaModel::ActionDrain(3), 5.0f);
			TestEqual(TEXT("melee heavy"), UEmergeStaminaModel::ActionDrain(5), 25.0f);
			TestEqual(TEXT("evade"), UEmergeStaminaModel::ActionDrain(6), 8.0f);
			TestEqual(TEXT("unknown -> 0"), UEmergeStaminaModel::ActionDrain(99), 0.0f);
		});
	});

	Describe("RegenPerSec", [this]()
	{
		It("state-dependent regen", [this]()
		{
			TestEqual(TEXT("idle -> 5"), UEmergeStaminaModel::RegenPerSec(0), 5.0f);
			TestEqual(TEXT("walk -> 4"), UEmergeStaminaModel::RegenPerSec(1), 4.0f);
			TestEqual(TEXT("jog -> 2"), UEmergeStaminaModel::RegenPerSec(2), 2.0f);
			TestEqual(TEXT("roll -> 0"), UEmergeStaminaModel::RegenPerSec(5), 0.0f);
			TestEqual(TEXT("unknown -> 0"), UEmergeStaminaModel::RegenPerSec(9), 0.0f);
		});
	});
}
#endif
