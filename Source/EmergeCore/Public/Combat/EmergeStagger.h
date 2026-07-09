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

// Designer-tunable stagger parameters (POD). Defaults are the exact golden values the acceptance
// specs verify, so a default-constructed struct reproduces the tested behavior bit-for-bit.
USTRUCT(BlueprintType)
struct FEmergeStaggerTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float HeadWeight = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float TorsoWeight = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float LegWeight = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float ArmWeight = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float FrontFactor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float OpposingFactor = 1.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float BehindFactor = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float StumbleThreshold = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float StaggerThreshold = 80.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float KnockdownThreshold = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger") float CrawlSpeedMultiplier = 0.35f;
};

// Deterministic stumble/stagger rules core. Golden numbers live in FEmergeStaggerTuning defaults;
// the legacy statics delegate to the *Tuned versions with a default struct (identical behavior).
UCLASS()
class EMERGECORE_API UEmergeStagger : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Returns Damage * partWeight * dirFactor (golden defaults).
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float HitStaggerAdd(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir);

	// Maps a stagger meter value to its discrete state band (golden defaults).
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static EEmergeStaggerState StateForMeter(float Meter);

	// Locomotion speed multiplier for a given stagger state (golden defaults).
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float LocomotionSpeedMultiplier(EEmergeStaggerState State);

	// Stagger meter naturally recovers toward 0 when not being hit:
	// max(0, Meter - max(0,RecoveryPerSec) * max(0,DeltaSeconds)).
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float MeterAfterRecovery(float Meter, float RecoveryPerSec, float DeltaSeconds);

	// Tuned variants: same math driven by a designer-supplied tuning struct.
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float HitStaggerAddTuned(const FEmergeStaggerTuning& Tuning, float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir);

	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static EEmergeStaggerState StateForMeterTuned(const FEmergeStaggerTuning& Tuning, float Meter);

	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static float LocomotionSpeedMultiplierTuned(const FEmergeStaggerTuning& Tuning, EEmergeStaggerState State);

	// ALS locomotion bridge mapping: 0 = ragdoll (knockdown/dead), 1 = forced walk
	// (stumble/stagger/crawl), 2 = run, 3 = sprint (only when wanted AND stamina allows).
	UFUNCTION(BlueprintCallable, Category = "Combat|Stagger")
	static int32 MobilityForState(EEmergeStaggerState State, bool bWantsSprint, bool bCanSprint);
};
