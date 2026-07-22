// HUMAN-AUTHORED trusted acceptance spec. Feature: stamina tied to encumbrance load tier.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeStaminaComponent.h"

BEGIN_DEFINE_SPEC(FEmergeStaminaComponentSpec, "Emergence.Survival.StaminaComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaminaComponentSpec)
void FEmergeStaminaComponentSpec::Define()
{
	Describe("Simulate drains on sprint (load-scaled) and regenerates otherwise", [this]()
	{
		It("heavier load drains faster; regen refills; clamped 0..100", [this]()
		{
			UEmergeStaminaComponent* C = NewObject<UEmergeStaminaComponent>();
			C->Stamina = 100.f; C->SprintDrainPerSec = 10.f; C->RegenPerSec = 5.f;

			// sprint, LoadTier 2 -> mult 2.0, dt 3 -> 100 - 10*2*3 = 40
			C->Simulate(3.0f, true, 2);
			TestEqual(TEXT("sprint heavy: 100 -> 40"), C->Stamina, 40.f);
			TestTrue(TEXT("can still sprint at 40"), C->CanSprint());

			// not sprinting, regen 5/s, dt 4 -> 40 + 20 = 60
			C->Simulate(4.0f, false, 0);
			TestEqual(TEXT("regen: 40 -> 60"), C->Stamina, 60.f);

			// over-drain floors at 0 and disables sprint
			C->Simulate(100.0f, true, 4);
			TestEqual(TEXT("floored at 0"), C->Stamina, 0.f);
			TestFalse(TEXT("cannot sprint at 0"), C->CanSprint());
		});
	});
}
#endif
