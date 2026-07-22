#include "Combat/EmergeEquipmentComponent.h"
#include "Combat/EmergeFactionTier.h"
#include "Combat/EmergeWeapon.h"

UEmergeEquipmentComponent::UEmergeEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UEmergeEquipmentComponent::EffectiveArmorValue() const
{
	return UEmergeFactionTier::ArmorValueForTier(ArmorTier);
}

float UEmergeEquipmentComponent::ShotInterval() const
{
	return UEmergeWeapon::TimeBetweenShots(WeaponRPM);
}

int32 UEmergeEquipmentComponent::Fire(int32 Rounds)
{
	const int32 Want = FMath::Max(0, Rounds);
	const int32 Fired = FMath::Min(Want, Loaded);
	Loaded = UEmergeWeapon::MagazineRemaining(Loaded, Fired);
	return Fired;
}

void UEmergeEquipmentComponent::Reload()
{
	Loaded = FMath::Max(0, MagazineSize);
}
