#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI/EmergeAITypes.h"
#include "EmergeEnemyController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

// Phase A enemy brain: AIPerception(Sight) is only the line-of-sight/in-cone GATE (UE Sight is binary and
// FAIStimulus.Strength is a broken placeholder). The graduated "slow identify" awareness comes from the
// nav-awareness kernel, driving a small FSM: Unaware->Suspicious->Investigating->Chasing->Searching->give-up.
UCLASS()
class EMERGECORE_API AEmergeEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEmergeEnemyController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
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
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float FillBaseRate = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float DecayRate = 0.25f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float ChaseSpeed = 450.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float ShambleSpeed = 150.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|AI") float GiveUpSeconds = 15.0f;

	UFUNCTION() void OnPerception(AActor* Actor, FAIStimulus Stimulus);

private:
	TWeakObjectPtr<APawn> Target;
	float SearchTime = 0.0f;
	void SetSpeed(float Speed);
	void EnsureMoveToActor(AActor* Goal);
	void EnsureMoveToLocation(const FVector& Dest, float Accept);
};
