#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "Combat/ArmorPenetrationResolver.h"
#include "Combat/ProjectileDefinition.h"
#include "Combat/ArmorItem.h"

// FORMULA (per hit):
//   zoneDmg   = baseDamage * zoneMultiplier
//   penetrated = projectilePenetration >= armorResistanceTier   (>= means threshold met)
//   if penetrated:  remaining = zoneDmg                          (armor bypassed, full zone damage)
//   else:           remaining = zoneDmg * (1 - armorMitigation)  (armorMitigation in [0,1])
//   remaining = FMath::Max(0.0f, remaining)
// Resolver signature: float UArmorPenetrationResolver::ResolveDamage(UProjectileDefinition*, UArmorItem*, float zoneMultiplier)

BEGIN_DEFINE_SPEC(FArmorPenetrationMitigationSpec, "Emergence.Items.ArmorPenetrationMitigation",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UProjectileDefinition* Projectile;
	UArmorItem* Armor;
	UArmorPenetrationResolver* Resolver;
END_DEFINE_SPEC(FArmorPenetrationMitigationSpec)

void FArmorPenetrationMitigationSpec::Define()
{
	BeforeEach([this]()
	{
		Projectile = NewObject<UProjectileDefinition>(GetTransientPackage());
		Armor      = NewObject<UArmorItem>(GetTransientPackage());
		Resolver   = NewObject<UArmorPenetrationResolver>(GetTransientPackage());
		Projectile->BaseDamage       = 100.0f;
		Armor->ResistanceTier        = 50.0f;
		Armor->Mitigation            = 0.25f; // blocks 25% when not penetrated
	});

	It("mitigates when penetration is below the resistance tier", [this]()
	{
		Projectile->Penetration = 49.0f; // one-under threshold => not penetrated
		// zone 2.0 => 100*2 = 200; 200*(1-0.25) = 150
		TestEqual(TEXT("Below tier"), Resolver->ResolveDamage(Projectile, Armor, 2.0f), 150.0f);
	});

	It("passes full zone damage when penetration exactly meets the tier", [this]()
	{
		Projectile->Penetration = 50.0f; // == threshold => penetrated
		// 100*1.5 = 150; full through
		TestEqual(TEXT("Exact tier"), Resolver->ResolveDamage(Projectile, Armor, 1.5f), 150.0f);
	});

	It("passes full zone damage when penetration exceeds the tier", [this]()
	{
		Projectile->Penetration = 999.0f; // max-ish => penetrated
		// 100*1.0 = 100
		TestEqual(TEXT("Above tier"), Resolver->ResolveDamage(Projectile, Armor, 1.0f), 100.0f);
	});

	It("returns zero for a zero zone multiplier boundary", [this]()
	{
		Projectile->Penetration = 0.0f; // not penetrated
		// 100*0 = 0; 0*(1-0.25) = 0
		TestEqual(TEXT("Zero zone"), Resolver->ResolveDamage(Projectile, Armor, 0.0f), 0.0f);
	});
}

#endif // WITH_AUTOMATION_TESTS