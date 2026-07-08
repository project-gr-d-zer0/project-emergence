// HUMAN-AUTHORED trusted acceptance spec. Feature: combat damage pipeline (ballistics+armor+stagger).
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeDamageComponent.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"

BEGIN_DEFINE_SPEC(FEmergeDamageComponentSpec, "Emergence.Combat.DamageComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDamageComponentSpec)
void FEmergeDamageComponentSpec::Define()
{
	Describe("ResolveHit composes penetration + mitigation + stagger", [this]()
	{
		It("penetrating torso hit: 50 raw, 40 armor -> 40 dmg + 40 stagger", [this]()
		{
			// Penetrates(30, class 2): 30 >= 20 -> true. MitigatedDamage(50,40,true)=50*(1-0.2)=40.
			// HitStaggerAdd(40, Torso, Front)=40.
			const FEmergeHitResult R = UEmergeDamageComponent::ResolveHit(50.f, 30.f, 2, 40.f,
				EEmergeBodyPart::Torso, EEmergeHitDir::Front);
			TestTrue(TEXT("penetrated"), R.bPenetrated);
			TestEqual(TEXT("final damage 40"), R.FinalDamage, 40.f);
			TestEqual(TEXT("stagger add 40"), R.StaggerAdd, 40.f);
		});

		It("stopped hit: 50 raw, 40 armor -> 15% bleed 7.5 dmg", [this]()
		{
			// Penetrates(10, class 2): 10 >= 20 -> false. MitigatedDamage(50,40,false)=50*0.15=7.5.
			const FEmergeHitResult R = UEmergeDamageComponent::ResolveHit(50.f, 10.f, 2, 40.f,
				EEmergeBodyPart::Torso, EEmergeHitDir::Front);
			TestFalse(TEXT("stopped"), R.bPenetrated);
			TestEqual(TEXT("final damage 7.5"), R.FinalDamage, 7.5f);
			TestEqual(TEXT("stagger add 7.5"), R.StaggerAdd, 7.5f);
		});
	});

	Describe("ApplyHit mutates target components", [this]()
	{
		It("subtracts health and adds stagger meter", [this]()
		{
			UEmergeDamageComponent* D = NewObject<UEmergeDamageComponent>();
			UEmergeVitalsComponent* V = NewObject<UEmergeVitalsComponent>();
			UEmergeStaggerComponent* S = NewObject<UEmergeStaggerComponent>();
			V->Health = 100.f; S->Meter = 0.f;
			FEmergeHitResult R; R.bPenetrated = true; R.FinalDamage = 40.f; R.StaggerAdd = 40.f;
			D->ApplyHit(R, V, S);
			TestEqual(TEXT("health 100-40 -> 60"), V->Health, 60.f);
			TestEqual(TEXT("meter 0+40 -> 40"), S->Meter, 40.f);
			// null targets are safely ignored
			D->ApplyHit(R, nullptr, nullptr);
			TestEqual(TEXT("null-safe: health unchanged"), V->Health, 60.f);
		});
	});
}
#endif
