#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBallistics.generated.h"

// Designer-tunable ballistics parameters (POD). Defaults are the exact golden values the acceptance
// specs verify, so a default-constructed struct reproduces the tested behavior bit-for-bit.
USTRUCT(BlueprintType)
struct FEmergeBallisticsTuning
{
	GENERATED_BODY()

	// Armor stops the round when PenetrationValue < ArmorClass * PenPerArmorClass.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Ballistics") float PenPerArmorClass = 10.0f;

	// Fraction of raw damage that bleeds through when a round is stopped.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Ballistics") float BleedThroughFrac = 0.15f;

	// Residual reduction on penetration: reduce = clamp(ArmorValue / Divisor, 0, MaxReduceFrac).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Ballistics") float ArmorReduceDivisor = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Ballistics") float MaxReduceFrac = 0.6f;
};

// Core ballistics formulas: penetration vs armor class, range-based damage falloff, and armor
// mitigation. Golden numbers live in FEmergeBallisticsTuning defaults; legacy statics delegate
// to the *Tuned versions with a default struct (identical behavior).
UCLASS()
class EMERGECORE_API UEmergeBallistics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Armor stops the round when PenetrationValue < ArmorClass * 10 (golden defaults).
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static bool Penetrates(float PenetrationValue, int32 ArmorClass);

	// Linear damage falloff: full damage up to FalloffStartM, decaying to 50% at/after FalloffEndM.
	// If FalloffEndM <= FalloffStartM, returns BaseDamage (degenerate range guard).
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static float ComputeDamage(float BaseDamage, float DistanceM, float FalloffStartM, float FalloffEndM);

	// Armor mitigation: stopped rounds bleed 15% through; penetrating rounds are reduced by residual
	// armor up to 60% (golden defaults).
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static float MitigatedDamage(float RawDamage, float ArmorValue, bool bPenetrated);

	// Tuned variants: same math driven by a designer-supplied tuning struct.
	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static bool PenetratesTuned(const FEmergeBallisticsTuning& Tuning, float PenetrationValue, int32 ArmorClass);

	UFUNCTION(BlueprintCallable, Category = "Ballistics")
	static float MitigatedDamageTuned(const FEmergeBallisticsTuning& Tuning, float RawDamage, float ArmorValue, bool bPenetrated);
};
