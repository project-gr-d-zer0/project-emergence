#include "AI/EmergeEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Nav/EmergeAwareness.h"

AEmergeEnemyController::AEmergeEnemyController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	PrimaryActorTick.bCanEverTick = true;

	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;                 // > SightRadius: detection hysteresis
	SightConfig->PeripheralVisionAngleDegrees = VisionHalfAngle;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;  // no teams set up yet -> detect the player
	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(UAISense_Sight::StaticClass());
	SetPerceptionComponent(*Perception);
}

void AEmergeEnemyController::BeginPlay()
{
	Super::BeginPlay();
	if (Perception)
	{
		Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEmergeEnemyController::OnPerception);
	}
	Target = UGameplayStatics::GetPlayerPawn(this, 0);
	SetSpeed(ShambleSpeed);
}

void AEmergeEnemyController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Target.IsValid() || Actor != Target.Get()) { return; }
	bTargetVisible = Stimulus.WasSuccessfullySensed();            // Sight is the LoS/in-cone gate
	if (bTargetVisible) { LastKnownPosition = Actor->GetActorLocation(); }
}

void AEmergeEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	APawn* Self = GetPawn();
	if (!Self || !Target.IsValid()) { return; }

	const float Dist = FVector::Dist(Self->GetActorLocation(), Target->GetActorLocation());

	// Graduated "slow identify" via the nav-awareness kernel: fill when visible (faster when close), decay otherwise.
	if (bTargetVisible)
	{
		const float Rate = UEmergeAwareness::FillRate(Dist, SightRadius, true) * FillBaseRate;
		Awareness = UEmergeAwareness::Accumulate(Awareness, Rate, DeltaSeconds);
		LastKnownPosition = Target->GetActorLocation();
	}
	else
	{
		Awareness = UEmergeAwareness::Decay(Awareness, DecayRate, DeltaSeconds);
	}

	const int32 StateCode = UEmergeAwareness::StateFor(Awareness);   // 0 Unaware,1 Suspicious,2 Alerted,3 Chasing

	if (Awareness >= 1.0f && bTargetVisible)
	{
		AIState = EEmergeAIState::Chasing;
		SearchTime = 0.0f;
		SetSpeed(ChaseSpeed);
		EnsureMoveToActor(Target.Get());
	}
	else if (AIState == EEmergeAIState::Chasing || AIState == EEmergeAIState::Searching)
	{
		// Lost the target (out of sight or awareness fell): hunt the last-known spot, then give up.
		AIState = EEmergeAIState::Searching;
		SetSpeed(ShambleSpeed);
		SearchTime += DeltaSeconds;
		EnsureMoveToLocation(LastKnownPosition, 100.0f);
		if (SearchTime >= GiveUpSeconds || Awareness <= 0.0f)
		{
			AIState = EEmergeAIState::Unaware;
			SearchTime = 0.0f;
			StopMovement();
		}
	}
	else if (StateCode >= 1)
	{
		AIState = (StateCode == 1) ? EEmergeAIState::Suspicious : EEmergeAIState::Investigating;
		SetSpeed(ShambleSpeed);
		if (!LastKnownPosition.IsZero()) { EnsureMoveToLocation(LastKnownPosition, 150.0f); }
	}
	else
	{
		AIState = EEmergeAIState::Unaware;
		SetSpeed(ShambleSpeed);
	}
}

void AEmergeEnemyController::EnsureMoveToActor(AActor* Goal)
{
	// Issue once; MoveToActor observes the goal actor and repaths as it moves. Re-issue only if not moving.
	if (GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		MoveToActor(Goal, 100.0f, true, true, false, nullptr, true);
	}
}

void AEmergeEnemyController::EnsureMoveToLocation(const FVector& Dest, float Accept)
{
	if (GetMoveStatus() != EPathFollowingStatus::Moving)
	{
		MoveToLocation(Dest, Accept, true, true, false, false, nullptr, true);
	}
}

void AEmergeEnemyController::SetSpeed(float Speed)
{
	if (ACharacter* C = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* M = C->GetCharacterMovement()) { M->MaxWalkSpeed = Speed; }
	}
}

float AEmergeEnemyController::GetDistanceToTarget() const
{
	const APawn* Self = GetPawn();
	if (!Self || !Target.IsValid()) { return -1.0f; }
	return FVector::Dist(Self->GetActorLocation(), Target->GetActorLocation());
}

FString AEmergeEnemyController::GetAIStatus()
{
	static const TCHAR* Names[] = { TEXT("Unaware"), TEXT("Suspicious"), TEXT("Investigating"), TEXT("Chasing"), TEXT("Searching") };
	const int32 Idx = FMath::Clamp((int32)AIState, 0, 4);
	return FString::Printf(
		TEXT("{\"state\":\"%s\",\"awareness\":%.2f,\"targetVisible\":%s,\"dist\":%.0f,\"lastKnown\":[%.0f,%.0f],\"searchTime\":%.1f}"),
		Names[Idx], Awareness, bTargetVisible ? TEXT("true") : TEXT("false"),
		GetDistanceToTarget(), LastKnownPosition.X, LastKnownPosition.Y, SearchTime);
}
