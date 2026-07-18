#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeArmorPenetrationResolver.generated.h"

class UEmergeProjectileDefinition;
class UEmergeArmorItem;

// Resolves per-hit damage after armor penetration/mitigation is applied.
// zoneDmg   = baseDamage * zoneMultiplier
// penetrated = projectilePenetration >= armorResistanceTier (>= means threshold met)
// remaining = penetrated ? zoneDmg : zoneDmg * (1 - armorMitigation), clamped to >= 0.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeArmorPenetrationResolver : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	float ResolveDamage(UEmergeProjectileDefinition* Projectile, UEmergeArmorItem* Armor, float ZoneMultiplier) const;
};
