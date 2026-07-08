#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeArmorWear.generated.h"

// Armor durability degradation: worn armor protects less; penetrating rounds chew less durability
// than blocked ones (they pass through); repair permanently lowers max durability.
UCLASS()
class EMERGECORE_API UEmergeArmorWear : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Armor scaled by current durability fraction; zero max durability yields zero protection.
	UFUNCTION(BlueprintCallable, Category = "Armor")
	static float EffectiveProtection(float ArmorValue, float CurDurability, float MaxDurability);

	// Penetrating rounds pass through, chewing 15% less durability than blocked ones; minimum 1 per hit.
	UFUNCTION(BlueprintCallable, Category = "Armor")
	static float DurabilityLoss(bool bPenetrated, float AmmoArmorDamage);

	// Each repair permanently lowers max durability by 10%.
	UFUNCTION(BlueprintCallable, Category = "Armor")
	static float MaxAfterRepair(float MaxDurability);
};
