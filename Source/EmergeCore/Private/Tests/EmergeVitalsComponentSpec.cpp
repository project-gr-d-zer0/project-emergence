// HUMAN-AUTHORED trusted acceptance spec. Feature: runtime vitals component wraps the tested kernel.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeVitalsComponent.h"

BEGIN_DEFINE_SPEC(FEmergeVitalsComponentSpec, "Emergence.Survival.VitalsComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeVitalsComponentSpec)
void FEmergeVitalsComponentSpec::Define()
{
	Describe("Simulate advances vitals via the tested kernel", [this]()
	{
		It("drains hydration by rate*dt then starves health at zero (exact golden)", [this]()
		{
			UEmergeVitalsComponent* C = NewObject<UEmergeVitalsComponent>();
			C->Hydration = 10.0f; C->HydrationDrainPerSec = 2.0f; C->StarveDamagePerSec = 5.0f; C->Health = 100.0f;

			C->Simulate(3.0f);                                  // 10 - 2*3 = 4, not starving
			TestEqual(TEXT("hydration 10-6 -> 4"), C->Hydration, 4.0f);
			TestEqual(TEXT("health unchanged"), C->Health, 100.0f);
			TestFalse(TEXT("not starving at 4"), C->IsStarving());

			C->Simulate(5.0f);                                  // 4 - 2*5 -> 0 (clamped); starving -> 5*5=25 dmg
			TestEqual(TEXT("hydration floored at 0"), C->Hydration, 0.0f);
			TestTrue(TEXT("starving at 0"), C->IsStarving());
			TestEqual(TEXT("health 100-25 -> 75"), C->Health, 75.0f);
		});
	});
}
#endif
