#pragma once
#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "EmergeEnemy.generated.h"

class UAlsMovementSettings;
class UAlsMantlingSettings;
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
	virtual UAlsMantlingSettings* SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType) override;

	// Anim-state oracle for RC (debugging pose/anim issues).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Anim")
	FString GetAnimDebug() const;

	// Single source of truth for zombie gait speeds (the controller derives its thresholds from
	// these in OnPossess). Research-tuned: shamble 150, chase 560 (vs player run 375/sprint 640).
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieWalkSpeed = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieRunSpeed = 560.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieSprintSpeed = 650.0f;

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

private:
	float LastChaseSpeedScale = -1.0f;   // last applied scale; re-apply only on >0.02 change (no per-tick churn)
};
