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
class EMERGECORE_API UEmergeWorldSense : public UTickableWorldSubsystem
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

private:
	TMap<FObjectKey, float> StuckSeconds;
	// Anim-liveness oracle: hand-bone movement relative to the actor (uu/s, smoothed). ~0 while the
	// pawn moves = frozen ref pose even if an anim instance is present.
	TMap<FObjectKey, FVector> PrevHandRel;
	TMap<FObjectKey, float> BoneMotionUu;
	bool bReady = false;
};
