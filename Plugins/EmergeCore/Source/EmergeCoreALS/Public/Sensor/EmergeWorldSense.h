#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/ObjectKey.h"
#include "EmergeWorldSense.generated.h"

class AEmergeEnemyController;

// World-level telemetry sensor: one RC-readable JSON snapshot of everything the player's own
// SenseEnvironment can't see — the AI population (census of every enemy: state/awareness/motion),
// navmesh readiness, the shared influence-grid belief, NPC glitch-oracles (stuck-while-pathing,
// below-kill-Z) and frame timing. Read via AEmergeCharacter::SenseWorld (proven RC path).
UCLASS()
class EMERGECOREALS_API UEmergeWorldSense : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return bReady; }

	UPROPERTY(EditAnywhere, Category = "Emerge|Sensor") float StuckSpeedFloor = 20.0f;  // uu/s: slower than this while pathing = not actually moving
	UPROPERTY(EditAnywhere, Category = "Emerge|Sensor") float StuckAlarmSeconds = 3.0f;

	// One JSON world snapshot: AI census + env + shared influence belief + anomalies.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Sensor")
	FString SenseWorld();

	// FULL skeleton of one character (actor name contains NameFilter; empty = the player):
	// every bone's world position + rotation. Burst-poll during traversal/anim windows to
	// reconstruct motion numerically (pelvis pitch = flip detection, limb trajectories, etc).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Sensor")
	FString SenseSkeleton(const FString& NameFilter);

private:
	TMap<FObjectKey, float> StuckSeconds;
	// Anim-liveness oracle: hand-bone movement relative to the actor (uu/s, smoothed). ~0 while the
	// pawn moves = frozen ref pose even if an anim instance is present.
	TMap<FObjectKey, FVector> PrevHandRel;
	TMap<FObjectKey, float> BoneMotionUu;
	// Foot-slide oracle ("unnatural run"): horizontal drift of the planted (lower) foot, uu/s smoothed.
	// A clean gait plants feet (~0 while grounded); fixed-rate loops at wrong speeds skate visibly.
	TMap<FObjectKey, FVector> PrevPlantedFoot;
	TMap<FObjectKey, float> FootSlideUu;
	void UpdateBoneOracles(const FObjectKey& Key, const APawn* P, float DeltaTime);
	bool bReady = false;
};
