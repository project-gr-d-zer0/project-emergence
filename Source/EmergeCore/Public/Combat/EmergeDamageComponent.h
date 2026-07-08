#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/EmergeStagger.h"   // EEmergeBodyPart / EEmergeHitDir
#include "EmergeDamageComponent.generated.h"

class UEmergeVitalsComponent;
class UEmergeStaggerComponent;

// Result of resolving one incoming hit through the combat pipeline.
USTRUCT(BlueprintType)
struct FEmergeHitResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Emerge|Damage")
	bool bPenetrated = false;

	UPROPERTY(BlueprintReadOnly, Category = "Emerge|Damage")
	float FinalDamage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Emerge|Damage")
	float StaggerAdd = 0.0f;
};

// Combat hit-resolution hub: composes the tested ballistics (penetration + armor mitigation) and stagger
// kernels into ONE deterministic hit result, then applies it to a target's vitals + stagger components.
// ResolveHit is pure/static so it is headless-testable; ApplyHit mutates the passed-in components.
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeDamageComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeDamageComponent();

	// Pure resolver: penetration -> armor-mitigated damage -> stagger contribution.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Damage")
	static FEmergeHitResult ResolveHit(float RawDamage, float PenetrationValue, int32 ArmorClass,
		float ArmorValue, EEmergeBodyPart Part, EEmergeHitDir Dir);

	// Applies a resolved hit: subtracts FinalDamage from Vitals health and adds StaggerAdd to the meter.
	// Either component may be null (a target without that system simply ignores that part of the hit).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Damage")
	void ApplyHit(const FEmergeHitResult& Hit, UEmergeVitalsComponent* Vitals, UEmergeStaggerComponent* Stagger);
};
