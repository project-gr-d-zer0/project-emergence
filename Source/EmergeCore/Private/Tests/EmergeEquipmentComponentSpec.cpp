// HUMAN-AUTHORED trusted acceptance spec. Feature: equipment loadout (armor tier + weapon).
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeEquipmentComponent.h"

BEGIN_DEFINE_SPEC(FEmergeEquipmentComponentSpec, "Emergence.Combat.EquipmentComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeEquipmentComponentSpec)
void FEmergeEquipmentComponentSpec::Define()
{
	Describe("Equipment exposes armor value, shot cadence, and magazine", [this]()
	{
		It("derives values from the tested kernels and tracks the magazine", [this]()
		{
			UEmergeEquipmentComponent* C = NewObject<UEmergeEquipmentComponent>();
			C->ArmorTier = 2; C->WeaponRPM = 600; C->MagazineSize = 30; C->Loaded = 30;

			TestEqual(TEXT("tier2 armor -> 40"), C->EffectiveArmorValue(), 40.f);   // clamp(2,0,4)*20
			TestEqual(TEXT("600rpm -> 0.1s"), C->ShotInterval(), 0.1f);             // 60/600

			TestEqual(TEXT("fire 5 -> 5 fired"), C->Fire(5), 5);
			TestEqual(TEXT("mag 30-5 -> 25"), C->Loaded, 25);
			TestEqual(TEXT("over-fire returns only remaining 25"), C->Fire(100), 25);
			TestEqual(TEXT("mag empty"), C->Loaded, 0);
			C->Reload();
			TestEqual(TEXT("reload -> 30"), C->Loaded, 30);
		});
	});
}
#endif
