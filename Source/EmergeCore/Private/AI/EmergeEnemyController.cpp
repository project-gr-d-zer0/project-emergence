#include "AI/EmergeEnemyController.h"
#include "AI/EmergeEnemy.h"
#include "Utility/AlsGameplayTags.h"
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
#include "Nav/EmergeCornering.h"
#include "Nav/EmergeHop.h"
#include "Nav/EmergeInfluenceGrid.h"
#include "Components/CapsuleComponent.h"

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
	// De-robotize (research): DetourCrowd steering knobs — steer into corners early, offset from
	// the string-pulled corner-hugging line, decelerate into the goal.
	if (UCrowdFollowingComponent* Crowd = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		Crowd->SetCrowdAnticipateTurns(true);
		Crowd->SetCrowdPathOffset(true);
		Crowd->SetCrowdSlowdownAtGoal(true);
	}
	if (Perception)
	{
		Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEmergeEnemyController::OnPerception);
	}
	Target = UGameplayStatics::GetPlayerPawn(this, 0);
	Influence = GetWorld() ? GetWorld()->GetSubsystem<UEmergeInfluenceGrid>() : nullptr;
	SetSpeed(ShambleSpeed);
}

void AEmergeEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Single source of truth for speeds = the enemy pawn (its properties drive the actual ALS gait
	// table); the controller's UPROPERTY defaults only cover non-AEmergeEnemy pawns.
	if (const AEmergeEnemy* Enemy = Cast<AEmergeEnemy>(InPawn))
	{
		ChaseSpeed = Enemy->ZombieRunSpeed;
		ShambleSpeed = Enemy->ZombieWalkSpeed;
	}
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
	if (!Self) { return; }
	if (!Target.IsValid())
	{
		// Re-acquire rather than going permanently inert (the player pawn can be destroyed and
		// recreated mid-session — respawn, seamless travel, possession swap).
		Target = UGameplayStatics::GetPlayerPawn(this, 0);
		if (!Target.IsValid())
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("%s: no player pawn to target"), *GetName());
			return;
		}
	}

	const float Dist = FVector::Dist(Self->GetActorLocation(), Target->GetActorLocation());

	// Debug omniscience (temporary until the vision/hearing awareness zones phase): force the
	// perception INPUT true each update, level-triggered — never pin the FSM state. Everything
	// downstream (awareness ramp, grid stamping, transitions) runs normally on always-seen data;
	// the seen branch below already stamps the influence grid EVERY update (level-triggered,
	// verified), so no extra stamp guard is needed.
	if (bDebugOmniscient) { bTargetVisible = true; Awareness = 1.0f; }   // pins awareness too: the
	// distance-curved fill never completes against a fleeing target (measured: zombie stuck
	// Suspicious at 3000+uu while "seeing" him) — omniscient means RELENTLESS, not just sighted.

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
		// Cornering penalty: slow while the heading sweeps hard (the player's juke/parkour escape margin).
		// The scale CANNOT ride through SetSpeed: that only maps the request to a GAIT, and ALS
		// recomputes MaxWalkSpeed from the gait table every physics tick — 560*0.65 still picked
		// Running at full 560 (measured no-op). Push the scale into the enemy's duplicated movement
		// settings instead; SetSpeed keeps only the gait decision.
		UpdateCorneringScale(Self, DeltaSeconds);
		if (AEmergeEnemy* Enemy = Cast<AEmergeEnemy>(Self))
		{
			Enemy->SetChaseSpeedScale(CornerScale);
		}
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

	// Traversal: minor props don't cut the navmesh, so the path can run straight into them.
	TryTraversalHop(Self, DeltaSeconds);
}

void AEmergeEnemyController::TryTraversalHop(APawn* Self, float DeltaSeconds)
{
	HopCooldown = FMath::Max(0.0f, HopCooldown - DeltaSeconds);
	AAlsCharacter* AlsChar = Cast<AAlsCharacter>(Self);
	if (!AlsChar || HopCooldown > 0.0f || GetMoveStatus() != EPathFollowingStatus::Moving) { return; }
	UCharacterMovementComponent* Move = AlsChar->GetCharacterMovement();
	if (!Move || !Move->IsMovingOnGround()) { return; }
	if (AlsChar->GetLocomotionAction() == AlsLocomotionActionTags::Mantling) { return; }

	// Probe along travel direction (velocity when moving, facing when pinned against the obstacle).
	const FVector Vel = AlsChar->GetVelocity();
	FVector Ahead = (Vel.SizeSquared2D() > 400.0f) ? FVector(Vel.X, Vel.Y, 0.0f).GetSafeNormal()
	                                               : FVector(AlsChar->GetActorForwardVector().X, AlsChar->GetActorForwardVector().Y, 0.0f).GetSafeNormal();
	if (Ahead.IsNearlyZero()) { return; }
	const float HalfH = AlsChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Feet = AlsChar->GetActorLocation() - FVector(0.0f, 0.0f, HalfH);
	FCollisionQueryParams QP(SCENE_QUERY_STAT(EmergeHopProbe), false, AlsChar);
	FHitResult LowHit, HighHit;
	const bool bLow = GetWorld()->LineTraceSingleByChannel(LowHit,
		Feet + FVector(0, 0, 45.0f), Feet + FVector(0, 0, 45.0f) + Ahead * HopTriggerDist, ECC_Visibility, QP);
	const bool bHigh = GetWorld()->LineTraceSingleByChannel(HighHit,
		Feet + FVector(0, 0, HopClearHeightUu + 15.0f), Feet + FVector(0, 0, HopClearHeightUu + 15.0f) + Ahead * (HopTriggerDist + 60.0f), ECC_Visibility, QP);
	if (bLow && Move->IsWalkable(LowHit)) { return; }   // ramp/stair, not a mantle target
	const float LowDist = bLow ? LowHit.Distance : -1.0f;
	if (UEmergeHop::ShouldHop(LowDist, HopTriggerDist, !bHigh, true))
	{
		// Zombie-look mode has no ALS anim instance (mantle montages can't play — the pawn refuses
		// via IsMantlingAllowedToStart): scripted fall-over-the-wall traversal instead. A slow
		// topple, never a jump (design canon).
		AEmergeEnemy* Zombie = Cast<AEmergeEnemy>(AlsChar);
		if (Zombie && Zombie->IsZombieLookActive())
		{
			if (Zombie->IsFallTraversing()) { return; }
			// Obstacle top: down-trace from the height the high probe proved clear, just past the
			// blocked knee-hit face (ShouldHop guarantees LowHit is a real within-range hit).
			const FVector TopStart = FVector(LowHit.ImpactPoint.X, LowHit.ImpactPoint.Y,
				Feet.Z + HopClearHeightUu + 15.0f) + Ahead * 10.0f;
			FHitResult TopHit;
			if (!GetWorld()->LineTraceSingleByChannel(TopHit, TopStart,
				TopStart - FVector(0.0f, 0.0f, HopClearHeightUu + 15.0f), ECC_Visibility, QP)) { return; }
			// Far-side floor: forward of the lip, straight down. No floor within 250uu below the
			// top = ledge/pit, abort — zombies topple over walls, they don't leap into holes.
			const FVector LandStart = TopHit.ImpactPoint + Ahead * 120.0f + FVector(0.0f, 0.0f, 20.0f);
			FHitResult FloorHit;
			if (!GetWorld()->LineTraceSingleByChannel(FloorHit, LandStart,
				LandStart - FVector(0.0f, 0.0f, 270.0f), ECC_Visibility, QP)) { return; }
			if (Zombie->StartFallTraversal(TopHit.ImpactPoint, FloorHit.ImpactPoint))
			{
				HopCooldown = HopCooldownSeconds;
				++HopCount;
			}
			return;
		}
		// The IDENTICAL mantle the player uses — ALS traces/validates the ledge itself.
		if (AlsChar->IsMantlingAllowedToStart() && AlsChar->StartMantling())
		{
			HopCooldown = HopCooldownSeconds;
			++HopCount;
		}
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

void AEmergeEnemyController::UpdateCorneringScale(const APawn* Self, float DeltaSeconds)
{
	const FVector Vel = Self->GetVelocity();
	// Recover toward full speed; a fresh hard sweep can always pull the scale down instantly.
	const float Recovered = (CornerRecoverSeconds > 0.0f)
		? FMath::Lerp(CornerScale, 1.0f, FMath::Clamp(DeltaSeconds / CornerRecoverSeconds, 0.0f, 1.0f))
		: 1.0f;
	if (Vel.SizeSquared2D() < 100.0f)
	{
		PrevHeading = FVector2D::ZeroVector;
		CornerScale = Recovered;
		return;
	}
	const FVector2D Heading = FVector2D(Vel.X, Vel.Y).GetSafeNormal();
	float Instant = 1.0f;
	if (!PrevHeading.IsNearlyZero() && DeltaSeconds > 0.0f)
	{
		const float Dot = FMath::Clamp(FVector2D::DotProduct(Heading, PrevHeading), -1.0f, 1.0f);
		const float SweepDegPerSec = FMath::RadiansToDegrees(FMath::Acos(Dot)) / DeltaSeconds;
		Instant = UEmergeCornering::SpeedScale(SweepDegPerSec, CornerStartDeg, CornerFullDeg, CornerMinScale);
	}
	PrevHeading = Heading;
	CornerScale = FMath::Min(Instant, Recovered);
}

void AEmergeEnemyController::SetSpeed(float Speed)
{
	// ALS owns MaxWalkSpeed (drives it from the character's gait settings): map the requested speed
	// to a gait. The threshold derives from the pawn's own speed tuning (single source of truth =
	// the enemy pawn's ZombieWalkSpeed/ZombieRunSpeed); 300 is the non-zombie fallback.
	if (AAlsCharacter* AlsChar = Cast<AAlsCharacter>(GetPawn()))
	{
		float GaitThreshold = 300.0f;
		if (const AEmergeEnemy* Enemy = Cast<AEmergeEnemy>(AlsChar))
		{
			GaitThreshold = (Enemy->ZombieWalkSpeed + Enemy->ZombieRunSpeed) * 0.5f;
		}
		AlsChar->SetDesiredGait(Speed > GaitThreshold ? AlsGaitTags::Running : AlsGaitTags::Walking);
		return;
	}
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
		TEXT("{\"state\":\"%s\",\"awareness\":%.2f,\"targetVisible\":%s,\"dist\":%.0f,\"lastKnown\":[%.0f,%.0f],\"searchTime\":%.1f,\"cornerScale\":%.2f,\"hops\":%d,\"grid\":%s}"),
		Names[Idx], Awareness, bTargetVisible ? TEXT("true") : TEXT("false"),
		GetDistanceToTarget(), LastKnownPosition.X, LastKnownPosition.Y, SearchTime, CornerScale, HopCount, *GridJson);
}
