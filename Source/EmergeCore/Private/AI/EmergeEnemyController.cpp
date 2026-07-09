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
#include "Nav/EmergeInfluenceGrid.h"

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
	Influence = GetWorld() ? GetWorld()->GetSubsystem<UEmergeInfluenceGrid>() : nullptr;
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

	// Point-blank presence: within PresenceRadius with clear line-of-sight counts as seen even
	// outside the vision cone (measured 2026-07-09: enemy gave up standing 69uu from the player
	// because it arrived facing past him). Zombies sense you at arm's length.
	const bool bSeen = bTargetVisible || (Dist < PresenceRadius && LineOfSightTo(Target.Get()));

	// Graduated "slow identify" via the nav-awareness kernel: curved ramp fills when visible
	// (quick up close, much slower at range), decays otherwise. Sightings stamp the shared belief grid.
	if (bSeen)
	{
		const float Rate = UEmergeAwareness::FillRateCurved(Dist, SightRadius, RampExponent, true) * FillBaseRate;
		Awareness = UEmergeAwareness::Accumulate(Awareness, Rate, DeltaSeconds);
		LastKnownPosition = Target->GetActorLocation();
		if (Influence) { Influence->Stamp(LastKnownPosition, Awareness); }
	}
	else
	{
		Awareness = UEmergeAwareness::Decay(Awareness, DecayRate, DeltaSeconds);
	}

	const int32 StateCode = UEmergeAwareness::StateFor(Awareness);   // 0 Unaware,1 Suspicious,2 Alerted,3 Chasing

	if (Awareness >= 1.0f && bSeen)
	{
		AIState = EEmergeAIState::Chasing;
		SearchTime = 0.0f;
		SetSpeed(ChaseSpeed);
		EnsureMoveToActor(Target.Get());
	}
	else if (AIState == EEmergeAIState::Chasing || AIState == EEmergeAIState::Searching)
	{
		// Lost the target: hunt the shared belief peak (drifts + spreads as info ages) instead of a
		// frozen last-known point; give up when the belief disperses or the timer runs out.
		if (AIState == EEmergeAIState::Chasing)
		{
			StopMovement();            // redirect from actor-chase to the belief peak immediately
			bIssuedMove = false;
		}
		AIState = EEmergeAIState::Searching;
		SetSpeed(ShambleSpeed);
		SearchTime += DeltaSeconds;
		const bool bBeliefAlive = Influence && !Influence->IsDispersed();
		EnsureMoveToLocation(bBeliefAlive ? Influence->PeakPosition() : LastKnownPosition, 100.0f);
		const bool bBeliefGone = Influence ? Influence->IsDispersed() : (Awareness <= 0.0f);
		if (SearchTime >= GiveUpSeconds || bBeliefGone)
		{
			AIState = EEmergeAIState::Unaware;
			SearchTime = 0.0f;
			StopMovement();
			bIssuedMove = false;
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
		bIssuedMove = false;   // actor-move: location drift tracking does not apply
	}
}

void AEmergeEnemyController::EnsureMoveToLocation(const FVector& Dest, float Accept)
{
	// Re-issue when idle OR when the destination has drifted (the belief peak moves as it diffuses).
	// Project to navmesh: grid peaks carry a synthetic Z.
	const bool bMoving = GetMoveStatus() == EPathFollowingStatus::Moving;
	const bool bDrifted = bMoving && bIssuedMove && FVector::Dist2D(Dest, IssuedMoveDest) > 200.0f;
	if (!bMoving || bDrifted)
	{
		MoveToLocation(Dest, Accept, true, true, true, false, nullptr, true);
		IssuedMoveDest = Dest;
		bIssuedMove = true;
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
	FString GridJson = TEXT("null");
	if (Influence)
	{
		const FVector Peak = Influence->PeakPosition();
		GridJson = FString::Printf(TEXT("{\"peak\":[%.0f,%.0f],\"peakConf\":%.3f,\"dispersed\":%s}"),
			Peak.X, Peak.Y, Influence->PeakConfidence(), Influence->IsDispersed() ? TEXT("true") : TEXT("false"));
	}
	return FString::Printf(
		TEXT("{\"state\":\"%s\",\"awareness\":%.2f,\"targetVisible\":%s,\"dist\":%.0f,\"lastKnown\":[%.0f,%.0f],\"searchTime\":%.1f,\"grid\":%s}"),
		Names[Idx], Awareness, bTargetVisible ? TEXT("true") : TEXT("false"),
		GetDistanceToTarget(), LastKnownPosition.X, LastKnownPosition.Y, SearchTime, *GridJson);
}
