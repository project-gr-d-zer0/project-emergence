#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI/EmergeAITypes.h"
#include "EmergeEnemyController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UEmergeInfluenceGrid;

// Phase A enemy brain: AIPerception(Sight) is only the line-of-sight/in-cone GATE (UE Sight is binary and
// FAIStimulus.Strength is a broken placeholder). The graduated "slow identify" awareness comes from the
// nav-awareness kernel, driving a small FSM: Unaware->Suspicious->Investigating->Chasing->Searching->give-up.
UCLASS()
class EMERGECOREALS_API AEmergeEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEmergeEnemyController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	// --- Telemetry (read over Remote Control) ---
	UPROPERTY(BlueprintReadOnly, Category = "Emerge|AI") float Awareness = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Emerge|AI") EEmergeAIState AIState = EEmergeAIState::Unaware;
	UPROPERTY(BlueprintReadOnly, Category = "Emerge|AI") bool bTargetVisible = false;
	UPROPERTY(BlueprintReadOnly, Category = "Emerge|AI") FVector LastKnownPosition = FVector::ZeroVector;

	UFUNCTION(BlueprintPure, Category = "Emerge|AI") float GetDistanceToTarget() const;
	UFUNCTION(BlueprintCallable, Category = "Emerge|AI") FString GetAIStatus();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Emerge|AI") TObjectPtr<UAIPerceptionComponent> Perception;
	UPROPERTY() TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float SightRadius = 1500.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float LoseSightRadius = 2000.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float VisionHalfAngle = 70.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float PresenceRadius = 250.0f;  // point-blank omnidirectional sense (no "forgot you at arm's length")
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float FillBaseRate = 0.4f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float RampExponent = 1.5f;  // >1 = slower identify at range (curved ramp)
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float DecayRate = 0.25f;
	// Research-tuned (2026-07-09): between player run (375) and sprint (640) — sprint is the escape
	// verb, but only while stamina lasts; the cornering penalty is the juke/parkour escape margin.
	// Defaults are fallbacks only: OnPossess re-initializes both from the possessed AEmergeEnemy's
	// ZombieRunSpeed/ZombieWalkSpeed (single source of truth = the enemy pawn).
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float ChaseSpeed = 560.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float ShambleSpeed = 150.0f;
	// Cornering penalty inputs are HEADING SWEEP RATES (deg/sec of velocity-direction change):
	// facing-vs-velocity error never exceeded ~20 deg on an AI pawn (measured), rates are the real signal.
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float CornerStartDeg = 60.0f;   // deg/s: below = full speed
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float CornerFullDeg = 180.0f;   // deg/s: at/above = MinScale
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float CornerMinScale = 0.65f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float CornerRecoverSeconds = 0.7f;  // dip hold: juke opens a real gap
	// Traversal: knee-blocked + head-clear ahead -> every pawn (ALS body OR compatible-skeletons
	// zombie mesh) runs the character's own ALS StartMantling — the IDENTICAL mantle the player
	// performs. ALS's own trace bands handle the height gating (~50uu walls up).
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float HopTriggerDist = 90.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float HopClearHeightUu = 140.0f;   // max mantleable band
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float HopCooldownSeconds = 1.2f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float GiveUpSeconds = 15.0f;

	// Omniscient tracking (temporary until the vision/hearing awareness zones phase): forces the
	// perception INPUTS (sight gate + presence) true every update, level-triggered — the awareness
	// ramp, influence-grid stamping, and FSM transitions all still run normally, just on
	// always-seen data. Never pins the FSM state directly.
	UPROPERTY(EditAnywhere, Category = "Emerge|Debug") bool bDebugOmniscient = false;

	UFUNCTION() void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	// Shared world belief: stamped on sight, searched (drifting peak) when the target is lost.
	UPROPERTY() TObjectPtr<UEmergeInfluenceGrid> Influence;

private:
	TWeakObjectPtr<APawn> Target;
	float SearchTime = 0.0f;
	FVector IssuedMoveDest = FVector::ZeroVector;
	bool bIssuedMove = false;
	float CornerScale = 1.0f;   // last applied cornering speed scale (telemetry)
	FVector2D PrevHeading = FVector2D::ZeroVector;
	float HopCooldown = 0.0f;
	int32 HopCount = 0;         // telemetry (mantles + fall traversals performed; JSON key stays "hops")
	void UpdateCorneringScale(const APawn* Self, float DeltaSeconds);
	void TryTraversalHop(APawn* Self, float DeltaSeconds);
	void SetSpeed(float Speed);
	void EnsureMoveToActor(AActor* Goal);
	void EnsureMoveToLocation(const FVector& Dest, float Accept);
};
