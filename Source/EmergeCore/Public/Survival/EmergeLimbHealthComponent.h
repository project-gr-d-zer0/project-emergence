#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeLimbHealthComponent.generated.h"

UENUM(BlueprintType)
enum class EEmergeLimb : uint8
{
	Head,
	Thorax,
	Stomach,
	LeftArm,
	RightArm,
	LeftLeg,
	RightLeg
};

// Per-limb health (Tarkov-style locational consequence). Max HP: head 35 / thorax 85 / stomach 70 /
// arms 60 / legs 65 (total 440). Head or thorax at 0 = death. Other limbs "black out" at 0 and stay
// usable-but-penalized; damage dealt to a blacked limb overflows to the surviving limbs, scaled by
// the hit limb's overflow multiplier (arm 0.49 / leg 0.7 / stomach 1.05), split evenly. Pure state
// math; headless-testable. Locational penalties (no-sprint, sway, crawl) are read by movement/weapon code.
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeLimbHealthComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeLimbHealthComponent();

	// Golden max HP for a limb: 35/85/70/60/60/65/65.
	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	static float MaxHPForLimb(EEmergeLimb Limb);

	// Overflow multiplier when a blacked limb is hit: arms 0.49, legs 0.7, stomach 1.05, head/thorax 1.0.
	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	static float OverflowMultiplier(EEmergeLimb Limb);

	// Head and thorax are fatal at 0; other limbs black out instead.
	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	static bool IsFatalLimb(EEmergeLimb Limb);

	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	float GetHP(EEmergeLimb Limb) const;

	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	bool IsBlacked(EEmergeLimb Limb) const;

	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	bool IsDead() const;

	// Both legs blacked -> locomotion should drop to the Crawl stagger state.
	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	bool BothLegsBlacked() const;

	UFUNCTION(BlueprintPure, Category = "Emerge|Limbs")
	float TotalHP() const;

	// Applies damage to a limb. Damage beyond the limb's remaining HP (or to an already-blacked limb)
	// overflows: excess * OverflowMultiplier(limb), split evenly across the other surviving limbs.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Limbs")
	void ApplyDamageToLimb(EEmergeLimb Limb, float Damage);

	// Reset all limbs to max (fresh body).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Limbs")
	void ResetLimbs();

private:
	// Indexed by (uint8)EEmergeLimb.
	UPROPERTY(VisibleAnywhere, Category = "Emerge|Limbs")
	TArray<float> LimbHP;
};
