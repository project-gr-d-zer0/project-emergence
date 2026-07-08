// HUMAN-AUTHORED trusted acceptance spec. Feature: per-limb HP with blackout + damage overflow.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeLimbHealthComponent.h"

BEGIN_DEFINE_SPEC(FEmergeLimbHealthSpec, "Emergence.Survival.LimbHealth",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeLimbHealthSpec)
void FEmergeLimbHealthSpec::Define()
{
	Describe("Fresh body golden values", [this]()
	{
		It("has 440 total across the 7 limbs", [this]()
		{
			UEmergeLimbHealthComponent* C = NewObject<UEmergeLimbHealthComponent>();
			TestEqual(TEXT("head 35"), C->GetHP(EEmergeLimb::Head), 35.f);
			TestEqual(TEXT("thorax 85"), C->GetHP(EEmergeLimb::Thorax), 85.f);
			TestEqual(TEXT("total 440"), C->TotalHP(), 440.f);
			TestFalse(TEXT("alive"), C->IsDead());
		});
	});

	Describe("Fatal limbs kill at zero", [this]()
	{
		It("thorax overkill = death, no overflow from fatal limbs", [this]()
		{
			UEmergeLimbHealthComponent* C = NewObject<UEmergeLimbHealthComponent>();
			C->ApplyDamageToLimb(EEmergeLimb::Thorax, 100.f);
			TestEqual(TEXT("thorax 0"), C->GetHP(EEmergeLimb::Thorax), 0.f);
			TestTrue(TEXT("dead"), C->IsDead());
			TestEqual(TEXT("head untouched (no fatal overflow)"), C->GetHP(EEmergeLimb::Head), 35.f);
		});
	});

	Describe("Blackout and overflow", [this]()
	{
		It("exact-kill leg has no overflow; hitting a blacked leg overflows 0.7x split 6 ways", [this]()
		{
			UEmergeLimbHealthComponent* C = NewObject<UEmergeLimbHealthComponent>();
			C->ApplyDamageToLimb(EEmergeLimb::LeftLeg, 65.f);   // exact kill, zero excess
			TestTrue(TEXT("left leg blacked"), C->IsBlacked(EEmergeLimb::LeftLeg));
			TestEqual(TEXT("no overflow: total 375"), C->TotalHP(), 375.f);
			TestFalse(TEXT("alive"), C->IsDead());
			TestFalse(TEXT("one leg -> not crawling"), C->BothLegsBlacked());

			// 60 dmg to the blacked leg: overflow 60*0.7=42, split across 6 survivors = 7 each.
			C->ApplyDamageToLimb(EEmergeLimb::LeftLeg, 60.f);
			TestEqual(TEXT("head 35-7 -> 28"), C->GetHP(EEmergeLimb::Head), 28.f);
			TestEqual(TEXT("thorax 85-7 -> 78"), C->GetHP(EEmergeLimb::Thorax), 78.f);
			TestEqual(TEXT("right leg 65-7 -> 58"), C->GetHP(EEmergeLimb::RightLeg), 58.f);
			TestEqual(TEXT("total 375-42 -> 333"), C->TotalHP(), 333.f);

			C->ApplyDamageToLimb(EEmergeLimb::RightLeg, 58.f);
			TestTrue(TEXT("both legs blacked -> crawl"), C->BothLegsBlacked());

			C->ResetLimbs();
			TestEqual(TEXT("reset -> 440"), C->TotalHP(), 440.f);
		});
	});
}
#endif
