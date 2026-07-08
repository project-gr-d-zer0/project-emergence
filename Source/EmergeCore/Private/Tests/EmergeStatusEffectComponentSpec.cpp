// HUMAN-AUTHORED trusted acceptance spec. Feature: bleeding status effect (medical -> vitals).
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeStatusEffectComponent.h"
#include "Survival/EmergeVitalsComponent.h"

BEGIN_DEFINE_SPEC(FEmergeStatusEffectComponentSpec, "Emergence.Survival.StatusEffectComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStatusEffectComponentSpec)
void FEmergeStatusEffectComponentSpec::Define()
{
	Describe("Bleeding drains vitals until treated", [this]()
	{
		It("bleeds health over time, then a bandage stops it", [this]()
		{
			UEmergeStatusEffectComponent* C = NewObject<UEmergeStatusEffectComponent>();
			UEmergeVitalsComponent* V = NewObject<UEmergeVitalsComponent>();
			V->Health = 100.f;

			C->StartBleeding(2.0f);
			const float d = C->Simulate(3.0f, V);          // BleedDamage(2,3,true)=6
			TestEqual(TEXT("bled 6"), d, 6.f);
			TestEqual(TEXT("health 100-6 -> 94"), V->Health, 94.f);

			TestTrue(TEXT("bandage stops bleeding"), C->ApplyMedical(1));
			const float d2 = C->Simulate(3.0f, V);         // not bleeding -> 0
			TestEqual(TEXT("no further bleed"), d2, 0.f);
			TestEqual(TEXT("health unchanged 94"), V->Health, 94.f);

			TestFalse(TEXT("non-medical item does not stop bleeding"), C->ApplyMedical(3));
		});
	});
}
#endif
