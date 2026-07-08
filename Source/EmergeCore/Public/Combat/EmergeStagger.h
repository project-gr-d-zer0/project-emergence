#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStagger.generated.h"

UENUM(BlueprintType)
enum class EEmergeBodyPart : uint8
{
	Head,
	Torso,
	Arm,
	Leg
};

UENUM(BlueprintType)
enum class EEmergeHitDir : uint8
{
	Front,
	Opposing,
	Behind
};

UENUM(BlueprintType)
enum class EEmergeStaggerState : uint8
{
	Moving,
	Stumble,
	Stagger,
	Knockdown,
	Crawl,
	Dead
};

// Deterministic stumble/stagger rules core. Data-driven tuning will move to a
// UPrimaryDataAsset later; first-pass golden numbers live here per acceptance spec.
UCLASS()
class EMERGECORE_API UEmergeStagger : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Returns Damage * partWeight * dirFactor.
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float HitStaggerAdd(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir);

	// Maps a stagger meter value to its discrete state band.
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static EEmergeStaggerState StateForMeter(float Meter);

	// Locomotion speed multiplier for a given stagger state.
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float LocomotionSpeedMultiplier(EEmergeStaggerState State);

	// Stagger meter naturally recovers toward 0 when not being hit:
	// max(0, Meter - max(0,RecoveryPerSec) * max(0,DeltaSeconds)).
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float MeterAfterRecovery(float Meter, float RecoveryPerSec, float DeltaSeconds);
};
