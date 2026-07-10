#pragma once
#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "EmergeEnemy.generated.h"

class UAlsMovementSettings;
class UAlsMantlingSettings;
class UAnimSequence;
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
	virtual bool IsMantlingAllowedToStart_Implementation() const override;

	// Anim-state oracle for RC (debugging pose/anim issues).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Anim")
	FString GetAnimDebug() const;

	// Scripted zombie wall traversal: a slow two-segment topple over a low obstacle — rise up the
	// wall face to above the obstacle top, then collapse forward/down to the far-side floor —
	// driven from Tick as a world-space capsule lerp. Zombie-look mode has no ALS anim instance
	// (mantle montages can't play, IsMantlingAllowedToStart refuses), so the controller calls this
	// instead of StartMantling; design canon: shamblers messily FALL over walls, they never jump.
	// Returns false unless zombie-look is active, grounded, and not already traversing.
	UFUNCTION(BlueprintCallable, Category = "Emerge|ZombieLook")
	bool StartFallTraversal(const FVector& ObstacleTopPoint, const FVector& LandingPoint);

	bool IsZombieLookActive() const { return bZombieLookActive; }
	bool IsFallTraversing() const { return bFallTraversing; }

	// Single source of truth for zombie gait speeds (the controller derives its thresholds from
	// these in OnPossess). Research-tuned: shamble 150, chase 560 (vs player run 375/sprint 640).
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieWalkSpeed = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieRunSpeed = 560.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge") float ZombieSprintSpeed = 650.0f;

	// Zombie visual + locomotion look (v1 single-node route): BeginPlay swaps the ALS body for the
	// MoCap Online zombie mesh and drives animation as speed-switched single-node playback (idle /
	// walk / chase variant per instance, play rate = actual speed / authored root-motion speed).
	// This deliberately bypasses AB_Als/ALS anim polish for the basic-mechanics test; mantling
	// anims are unavailable in this mode (flat-ground scenario), so mantling is disabled while
	// active. false = exactly the current ALS body + behavior (movement stays CMC-driven either
	// way — the clips are in-place, correct for CMC-driven AI per research).
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") bool bUseZombieLook = true;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TSoftObjectPtr<USkeletalMesh> ZombieMesh;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TArray<TSoftObjectPtr<UAnimSequence>> IdleClips;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TArray<TSoftObjectPtr<UAnimSequence>> WalkClips;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TArray<TSoftObjectPtr<UAnimSequence>> ChaseClips;

	// Fall-traversal composite (StartFallTraversal), all shambler-native — no jump clips anywhere:
	// rise = reach loop (clawing/dragging itself up the face), topple = stand-to-crawl collapse
	// over the lip, recover = crawl-to-stand scramble after landing. One of each rolled + rate-
	// randomized per traversal.
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TArray<TSoftObjectPtr<UAnimSequence>> TraversalReachClips;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TArray<TSoftObjectPtr<UAnimSequence>> TraversalToppleClips;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") TArray<TSoftObjectPtr<UAnimSequence>> TraversalRecoverClips;
	// Segment durations roughly track the clips at their randomized rates (slow topple, not a vault).
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") float FallRiseSeconds = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|ZombieLook") float FallDropSeconds = 0.9f;

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

	// Zombie-look runtime state. Hard refs to the rolled clips (loaded in BeginPlay) so GC can't
	// drop them out from under the single-node player.
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieIdleClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieWalkClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieChaseClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieCurrentClip;
	float ZombieWalkAuthoredSpeed = 80.0f;    // uu/s from the Root_Motion sibling (fallback 80)
	float ZombieChaseAuthoredSpeed = 350.0f;  // uu/s from the Root_Motion sibling (fallback 350)
	bool bZombieLookActive = false;           // mesh swap + single-node mode actually engaged
	bool bZombieFirstPlayDone = false;        // first-play random-phase desync issued

	// Fall-traversal runtime state (see StartFallTraversal). Hard clip refs for GC safety, same as
	// the locomotion clips above.
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieReachClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieToppleClip;
	UPROPERTY(Transient) TObjectPtr<UAnimSequence> ZombieRecoverClip;
	bool bFallTraversing = false;
	int32 FallPhase = 0;              // 0 rise-to-apex, 1 topple-to-floor, 2 scramble-up pause
	float FallPhaseTime = 0.0f;       // seconds into the current phase
	float FallElapsed = 0.0f;         // watchdog clock over the whole traversal
	float FallRecoverPause = 0.6f;    // rolled per traversal (shamblers struggle up at their own pace)
	FVector FallStart = FVector::ZeroVector;
	FVector FallApex = FVector::ZeroVector;
	FVector FallLand = FVector::ZeroVector;
	int32 FallTraversalCount = 0;     // telemetry (GetAnimDebug "fallTraversals")

	void SetupZombieLook();
	void UpdateZombieAnim();
	void TickFallTraversal(float DeltaSeconds);
	void FinishFallTraversal();
	void PlayTraversalClip(UAnimSequence* Clip, bool bLooping, float Rate);
	static float AuthoredSpeedFromRootMotion(const UAnimSequence* IpcClip, float FallbackSpeed);
};
