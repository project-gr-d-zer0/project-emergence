#pragma once
#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "EmergeEnemy.generated.h"

class UAlsMovementSettings;
class UAlsMantlingSettings;
class USkeletalMesh;
enum class EAlsMantlingType : uint8;

// Phase A enemy: a full ALS character — the SAME locomotion + mantle system as the player, so
// its runs are speed-matched (no foot-sliding fixed loops) and its wall traversal is the
// IDENTICAL mantle the player performs (clip-mimicry on the mannequin rig kept reading wrong:
// vault=flip, mantle-clip=sandbox dependency flood, hurdle=still not a mantle — all measured).
// Zombie speeds live in its own movement settings, fed from the Zombie*Speed properties below.
UCLASS()
class EMERGECORE_API AEmergeEnemy : public AAlsCharacter
{
	GENERATED_BODY()
public:
	AEmergeEnemy();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UAlsMantlingSettings* SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType) override;

	// Anim-state oracle for RC (debugging pose/anim issues).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Anim")
	FString GetAnimDebug() const;

	bool IsZombieLookActive() const { return bZombieLookActive; }

	// Single source of truth for zombie gait speeds (the controller derives its thresholds from
	// these in OnPossess). Research-tuned: shamble 150, chase 560 (vs player run 375/sprint 640).
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieWalkSpeed = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieRunSpeed = 560.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieSprintSpeed = 650.0f;

	// Zombie visual (v2 compatible-skeletons route): BeginPlay swaps the ALS body for the MoCap
	// Online zombie SK_Mannequin mesh and KEEPS the full ALS pipeline — AB_Als keeps running on
	// the swapped mesh because the zombie's UE4_Mannequin_Skeleton lists SK_Als as a compatible
	// skeleton (set both ways by emerge_py/compat_skeletons.py). All ALS state/gait/mantling
	// behaves exactly as on the ALS body; the zombie plays HUMAN ALS locomotion anims for now
	// (interim — the zombie clip layer comes next). false = the stock ALS body.
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") bool bUseZombieLook = true;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TSoftObjectPtr<USkeletalMesh> ZombieMesh;

	// Cornering slowdown: rewrites the run speeds in the DUPLICATED movement settings and re-hands
	// them to the ALS movement component. On ALS characters MaxWalkSpeed/MaxAcceleration/braking are
	// recomputed every physics tick from the UAlsMovementSettings gait table + curves — tuning goes
	// through the settings object + SetMovementSettings() (forces RefreshGaitSettings), never CMC
	// field writes (those are silently overwritten the same tick).
	UFUNCTION(BlueprintCallable, Category = "Emerge")
	void SetChaseSpeedScale(float Scale);

protected:
	UPROPERTY() TObjectPtr<UAlsMovementSettings> BaseMovementSettings;   // MS_Als_Normal (curves source)
	UPROPERTY() TObjectPtr<UAlsMovementSettings> ZombieMovementSettings; // runtime duplicate (speed/curve overrides)
	UPROPERTY() TObjectPtr<UAlsMantlingSettings> MantlingSettingsHigh;
	UPROPERTY() TObjectPtr<UAlsMantlingSettings> MantlingSettingsLow;

	// Zombie mantle VARIETY bank: 3 speed variants per height (rate-scaled duplicates of the ALS
	// mantle montages, built by emerge_py/make_zombie_mantles.py into /Game/ZombieAnim). When the
	// zombie look is active, SelectMantlingSettings rolls a random element PER MANTLE — that roll
	// is the variance. The single settings above stay as the non-zombie fallback.
	UPROPERTY() TArray<TObjectPtr<UAlsMantlingSettings>> ZombieMantleHigh;
	UPROPERTY() TArray<TObjectPtr<UAlsMantlingSettings>> ZombieMantleLow;

	// Trip-over pool (RamsterZ TripOverToCrawlIdle clips) — the PRIMARY zombie traversal look:
	// rolled at ZombieTripChance before the mantle variety bank is consulted at all.
	UPROPERTY() TArray<TObjectPtr<UAlsMantlingSettings>> ZombieTripHigh;
	UPROPERTY() TArray<TObjectPtr<UAlsMantlingSettings>> ZombieTripLow;
	UPROPERTY(EditAnywhere, Category = "Emerge|Zombie", meta = (ClampMin = 0, ClampMax = 1))
	float ZombieTripChance = 0.75f;

private:
	float LastChaseSpeedScale = -1.0f;   // last applied scale; re-apply only on >0.02 change (no per-tick churn)

	bool bZombieLookActive = false;      // zombie mesh swap actually engaged (ALS pipeline intact)
	double ZombieNextDebugTime = 0.0;    // EMERGE_ZLOOK heartbeat cadence (world seconds)

	void SetupZombieLook();
};
