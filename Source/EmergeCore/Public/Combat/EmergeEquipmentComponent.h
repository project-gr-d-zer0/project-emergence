#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeEquipmentComponent.generated.h"

// Runtime loadout: the equipped armor tier and weapon. Exposes the effective armor value that feeds the
// damage pipeline, the equipped weapon's shot cadence, and magazine tracking — all via the tested
// UEmergeFactionTier / UEmergeWeapon kernels. Pure state; headless-testable.
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeEquipmentComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Equipment")
	int32 ArmorTier = 0;      // 0 cloth .. 4 plate

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Equipment")
	int32 WeaponRPM = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Equipment")
	int32 MagazineSize = 30;

	UPROPERTY(BlueprintReadOnly, Category = "Emerge|Equipment")
	int32 Loaded = 30;

	// Armor value from the equipped armor tier (feeds UEmergeDamageComponent::ResolveHit).
	UFUNCTION(BlueprintPure, Category = "Emerge|Equipment")
	float EffectiveArmorValue() const;

	// Seconds between shots for the equipped weapon.
	UFUNCTION(BlueprintPure, Category = "Emerge|Equipment")
	float ShotInterval() const;

	// Fire up to Rounds shots; decrements the magazine. Returns rounds actually fired.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Equipment")
	int32 Fire(int32 Rounds);

	// Refill the magazine to MagazineSize.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Equipment")
	void Reload();
};
