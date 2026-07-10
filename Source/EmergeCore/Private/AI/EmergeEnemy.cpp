#include "AI/EmergeEnemy.h"
#include "AI/EmergeEnemyController.h"
#include "AlsCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Settings/AlsMantlingSettings.h"
#include "Utility/AlsGameplayTags.h"
#include "UObject/ConstructorHelpers.h"

AEmergeEnemy::AEmergeEnemy()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEmergeEnemyController::StaticClass();

	// ALS body: same mesh + anim blueprint family as the player -> identical animation quality.
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
			TEXT("/ALS/ALS/Character/SKM_Als.SKM_Als"));
		if (MeshAsset.Succeeded()) { MeshComp->SetSkeletalMesh(MeshAsset.Object); }
		static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/ALS/ALS/Character/AB_Als"));
		if (AnimBP.Succeeded()) { MeshComp->SetAnimInstanceClass(AnimBP.Class); }
	}

	// ALS gameplay settings (required or the animation refresh early-returns).
	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> AlsSettings(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (AlsSettings.Succeeded()) { Settings = AlsSettings.Object; }

	// Base movement asset (curves source); duplicated + speed-overridden in PostInitializeComponents
	// — the supported runtime pattern (a CDO-subobject DataAsset copy is not the idiom; researched).
	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> BaseMovement(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (BaseMovement.Succeeded()) { BaseMovementSettings = BaseMovement.Object; }

	// Same mantle as the player (montages + auto start time).
	MantlingSettingsHigh = CreateDefaultSubobject<UAlsMantlingSettings>(TEXT("MantlingSettingsHigh"));
	MantlingSettingsLow = CreateDefaultSubobject<UAlsMantlingSettings>(TEXT("MantlingSettingsLow"));
	MantlingSettingsHigh->bAutoCalculateStartTime = true;
	MantlingSettingsLow->bAutoCalculateStartTime = true;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MantleHigh(
		TEXT("/ALS/ALS/Animations/Actions/Mantle/AM_Als_Mantle_High.AM_Als_Mantle_High"));
	if (MantleHigh.Succeeded()) { MantlingSettingsHigh->Montage = MantleHigh.Object; }
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MantleLow(
		TEXT("/ALS/ALS/Animations/Actions/Mantle/AM_Als_Mantle_Low.AM_Als_Mantle_Low"));
	if (MantleLow.Succeeded()) { MantlingSettingsLow->Montage = MantleLow.Object; }

	// Zombie look defaults: MoCap Online Zombie Pro (UE4-Mannequin rig). Soft paths only — loaded
	// at runtime in SetupZombieLook via LoadSynchronous, NOT ConstructorHelpers (a hard find here
	// would drag every clip into memory with the CDO even with bUseZombieLook off).
	ZombieMesh = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(
		TEXT("/Game/Zombie_01/Character/Mesh/SK_Mannequin.SK_Mannequin")));
	const auto ZombieClip = [](const TCHAR* Name)
	{
		return TSoftObjectPtr<UAnimSequence>(FSoftObjectPath(
			FString::Printf(TEXT("/Game/Zombie_01/Animation/In_Place/%s.%s"), Name, Name)));
	};
	IdleClips = { ZombieClip(TEXT("Zombie_Idle_1_v2_IPC")), ZombieClip(TEXT("Zombie_Idle_2_IPC")),
	              ZombieClip(TEXT("Zombie_Idle_3_IPC")), ZombieClip(TEXT("Zombie_Idle_4_IPC")) };
	// Walk variants: the *_Loop_IPC walks (the Stand_Walk_F_* clips are stand-to-walk transition
	// starts, not loops — verified on disk) plus the two shamble loops for extra horde variety.
	WalkClips = { ZombieClip(TEXT("Zombie_Walk_F_1_Loop_IPC")), ZombieClip(TEXT("Zombie_Walk_F_2_Loop_IPC")),
	              ZombieClip(TEXT("Zombie_Walk_F_3_Loop_IPC")), ZombieClip(TEXT("Zombie_Walk_F_4_Loop_IPC")),
	              ZombieClip(TEXT("Zombie_Walk_F_5_Loop_IPC")), ZombieClip(TEXT("Zombie_Walk_F_6_Loop_IPC")),
	              ZombieClip(TEXT("Zombie_Shamble_1_IPC")), ZombieClip(TEXT("Zombie_Shamble_2_IPC")) };
	// Chase loops on disk are 1, 2, 3 and 5 — there is no Zombie_Chase_4_Loop_IPC (verified).
	ChaseClips = { ZombieClip(TEXT("Zombie_Chase_1_Loop_IPC")), ZombieClip(TEXT("Zombie_Chase_2_Loop_IPC")),
	               ZombieClip(TEXT("Zombie_Chase_3_Loop_IPC")), ZombieClip(TEXT("Zombie_Chase_5_Loop_IPC")) };
	// Fall-traversal composite (names verified on disk): reach loops = clawing up the wall face,
	// stand-to-crawl = collapsing forward over the lip, crawl-to-stand = the scramble back up.
	// Deliberately NO Jump_* clips — shamblers fall over walls, they never jump (design canon).
	TraversalReachClips = { ZombieClip(TEXT("Zombie_Reach_1_Loop_IPC")), ZombieClip(TEXT("Zombie_Reach_2_Loop_IPC")) };
	TraversalToppleClips = { ZombieClip(TEXT("Zombie_Stand_to_Crawl_1_IPC")), ZombieClip(TEXT("Zombie_Stand_to_Crawl_2_IPC")) };
	TraversalRecoverClips = { ZombieClip(TEXT("Zombie_Crawl_1_to_Stand_IPC")), ZombieClip(TEXT("Zombie_Crawl_2_to_Stand_IPC")) };
}

void AEmergeEnemy::PostInitializeComponents()
{
	// Zombie tuning: duplicate MS_Als_Normal and override BOTH the gait speeds (Walking = shamble,
	// Running = chase, from the Zombie*Speed properties) and the accel/decel/friction curve — ALS
	// recomputes MaxWalkSpeed/MaxAcceleration/braking every physics tick from this gait table +
	// curves, so direct CMC field writes are dead tuning; everything goes through this object.
	// DuplicateObject copies the table but SHARES the referenced UCurveVector asset, so the curve
	// gets its own duplicate too: X channel = acceleration, scaled by 0.35 for weighty zombie
	// starts (Y = deceleration and Z = ground friction stay stock — stops/traction unchanged).
	// Must happen BEFORE Super, which hands MovementSettings to the movement component.
	if (BaseMovementSettings)
	{
		UAlsMovementSettings* Zombie = DuplicateObject(BaseMovementSettings.Get(), this, TEXT("ZombieMovementSettings"));
		TMap<UCurveVector*, UCurveVector*> DuplicatedCurves;   // stances usually share one curve asset: duplicate once
		for (TPair<FGameplayTag, FAlsMovementStanceSettings>& RotationPair : Zombie->RotationModes)
		{
			for (TPair<FGameplayTag, FAlsMovementGaitSettings>& StancePair : RotationPair.Value.Stances)
			{
				StancePair.Value.WalkForwardSpeed = ZombieWalkSpeed;
				StancePair.Value.WalkBackwardSpeed = ZombieWalkSpeed;
				StancePair.Value.RunForwardSpeed = ZombieRunSpeed;
				StancePair.Value.RunBackwardSpeed = ZombieRunSpeed;
				StancePair.Value.SprintSpeed = ZombieSprintSpeed;
				if (UCurveVector* SourceCurve = StancePair.Value.AccelerationAndDecelerationAndGroundFrictionCurve)
				{
					UCurveVector*& Scaled = DuplicatedCurves.FindOrAdd(SourceCurve);
					if (!Scaled)
					{
						Scaled = DuplicateObject(SourceCurve, this);
						for (FRichCurveKey& Key : Scaled->FloatCurves[0].Keys)   // [0] = X = acceleration
						{
							Key.Value *= 0.35f;
						}
					}
					StancePair.Value.AccelerationAndDecelerationAndGroundFrictionCurve = Scaled;
				}
			}
		}
		ZombieMovementSettings = Zombie;
		MovementSettings = Zombie;
	}
	Super::PostInitializeComponents();
}

void AEmergeEnemy::SetChaseSpeedScale(float Scale)
{
	Scale = FMath::Clamp(Scale, 0.5f, 1.0f);
	// Re-apply only on a real change: rewriting the gait table + RefreshGaitSettings every tick
	// for sub-percent wiggle is pointless churn.
	if (!ZombieMovementSettings || FMath::Abs(Scale - LastChaseSpeedScale) <= 0.02f) { return; }
	LastChaseSpeedScale = Scale;
	const float ScaledRunSpeed = ZombieRunSpeed * Scale;
	for (TPair<FGameplayTag, FAlsMovementStanceSettings>& RotationPair : ZombieMovementSettings->RotationModes)
	{
		for (TPair<FGameplayTag, FAlsMovementGaitSettings>& StancePair : RotationPair.Value.Stances)
		{
			StancePair.Value.RunForwardSpeed = ScaledRunSpeed;
			StancePair.Value.RunBackwardSpeed = ScaledRunSpeed;
		}
	}
	// ALS caches the gait settings in the movement component and recomputes MaxWalkSpeed from that
	// cache every physics tick — the edit above is invisible until SetMovementSettings re-hands the
	// object over and forces RefreshGaitSettings.
	if (UAlsCharacterMovementComponent* Move = Cast<UAlsCharacterMovementComponent>(GetCharacterMovement()))
	{
		Move->SetMovementSettings(ZombieMovementSettings);
	}
}

void AEmergeEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Face travel direction (an AI has no meaningful view direction) and start at shamble.
	// NOTE: the desired rotation mode applies on the next locomotion refresh — there is a single
	// frame of ViewDirection facing first (cosmetic one-frame snap; documented so nobody chases it).
	SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
	SetDesiredGait(AlsGaitTags::Walking);
	// De-robotize (research quick wins): injured overlay = lurchy full-body posture for free.
	// Weighty starts come from the 0.35x-scaled acceleration curve baked into the duplicated
	// movement settings in PostInitializeComponents — NOT from CMC field writes here: ALS
	// recomputes MaxAcceleration/braking from its gait curves every physics tick (and the ALS
	// ctor already enables bUseAccelerationForPaths), so the old direct writes were dead code.
	SetOverlayMode(AlsOverlayModeTags::Injured);

	// Zombie look last: the mesh swap kills the ALS anim instance, so everything above that goes
	// through ALS state (rotation mode, gait, overlay) must already be set.
	if (bUseZombieLook)
	{
		SetupZombieLook();
	}
}

void AEmergeEnemy::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bZombieLookActive)
	{
		if (bFallTraversing)
		{
			TickFallTraversal(DeltaSeconds);
		}
		UpdateZombieAnim();
	}
}

void AEmergeEnemy::SetupZombieLook()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	USkeletalMesh* Zombie = ZombieMesh.LoadSynchronous();
	if (!MeshComp || !Zombie)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: zombie mesh failed to load - keeping the ALS body"), *GetName());
		return;
	}

	// Per-instance variety: each zombie rolls its own idle/walk/chase variant once at BeginPlay.
	ZombieIdleClip = IdleClips.Num() > 0 ? IdleClips[FMath::RandRange(0, IdleClips.Num() - 1)].LoadSynchronous() : nullptr;
	ZombieWalkClip = WalkClips.Num() > 0 ? WalkClips[FMath::RandRange(0, WalkClips.Num() - 1)].LoadSynchronous() : nullptr;
	ZombieChaseClip = ChaseClips.Num() > 0 ? ChaseClips[FMath::RandRange(0, ChaseClips.Num() - 1)].LoadSynchronous() : nullptr;
	if (!ZombieWalkClip || !ZombieChaseClip)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: zombie clips failed to load - keeping the ALS body"), *GetName());
		return;
	}

	// Authored travel speed per chosen clip, cached once (the IPC clips are in-place; the speed
	// lives in the Root_Motion sibling).
	ZombieWalkAuthoredSpeed = AuthoredSpeedFromRootMotion(ZombieWalkClip, 80.0f);
	ZombieChaseAuthoredSpeed = AuthoredSpeedFromRootMotion(ZombieChaseClip, 350.0f);

	// The swap destroys the UAlsAnimationInstance, and AAlsCharacter::Tick early-returns without
	// one — ALS rotation/gait refresh is dead from here on (by design for this v1: single-node
	// playback, no AB_Als polish, no mantling anims — flat-ground scenario). Take over the two
	// pieces the AI still needs: classic CMC orient-to-movement rotation here, and the
	// desired-gait -> movement-component push per tick in UpdateZombieAnim. All existing ALS
	// movement/settings logic (gait table, cornering scale, CMC driving) is untouched.
	MeshComp->SetSkeletalMesh(Zombie);
	MeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	MeshComp->SetUsingAbsoluteRotation(false);   // belt-and-braces: ALS flips this in its tick
	MeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -92.0f), FRotator(0.0f, -90.0f, 0.0f));
	AnimationInstance.Reset();   // make the ALS-tick early-return deterministic (no GC-timing limbo)
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = true;   // ALS forbids this at startup, but ALS rotation is dead now
		Move->RotationRate = FRotator(0.0f, 240.0f, 0.0f);
		// Curbs/street furniture are STEP-UPS, never traversals: make sure zombie-look mode walks
		// straight over kerb-height geometry (~20-50uu) so the controller's fall traversal (gated
		// at MinTraversalHeightUu) never has to fire on it. Nothing else in this mode touches step
		// height (the gait table/SetChaseSpeedScale only rewrite speeds + accel curves), but set it
		// explicitly rather than trusting the ALS default.
		Move->MaxStepHeight = 50.0f;
	}
	bZombieLookActive = true;
}

void AEmergeEnemy::UpdateZombieAnim()
{
	// Fall traversal owns the single node while active (reach/topple/recover clips) — the
	// speed-switched loop logic below would stomp them the same tick it ran.
	if (bFallTraversing) { return; }
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) { return; }

	// ALS's gait refresh is dead in this mode (see SetupZombieLook): push the desired gait (set by
	// the controller's SetSpeed) straight into the movement component, which still recomputes
	// MaxWalkSpeed from the zombie gait table every physics tick.
	if (AlsCharacterMovement) { AlsCharacterMovement->SetMaxAllowedGait(GetDesiredGait()); }

	// Speed-switched single-node playback: idle / walk variant / chase variant.
	const float Speed2D = GetVelocity().Size2D();
	UAnimSequence* DesiredClip;
	float AuthoredSpeed = 0.0f;   // 0 = fixed rate 1 (idle)
	if (Speed2D < 10.0f)
	{
		DesiredClip = ZombieIdleClip ? ZombieIdleClip.Get() : ZombieWalkClip.Get();
	}
	else if (Speed2D < (ZombieWalkSpeed + ZombieRunSpeed) * 0.5f)
	{
		DesiredClip = ZombieWalkClip;
		AuthoredSpeed = ZombieWalkAuthoredSpeed;
	}
	else
	{
		DesiredClip = ZombieChaseClip;
		AuthoredSpeed = ZombieChaseAuthoredSpeed;
	}
	if (!DesiredClip) { return; }

	// Re-issue on CHANGE only — plus whenever the node is not playing (self-heal, mirrors the old
	// proven pattern: anything that silently stops the single node gets restarted next tick).
	UAnimSingleNodeInstance* Node = MeshComp->GetSingleNodeInstance();
	if (DesiredClip != ZombieCurrentClip || !Node || !Node->IsPlaying())
	{
		MeshComp->PlayAnimation(DesiredClip, true);
		ZombieCurrentClip = DesiredClip;
		Node = MeshComp->GetSingleNodeInstance();
		if (!bZombieFirstPlayDone && Node)
		{
			// Desync the horde: random start phase so identical variants don't march in lockstep.
			Node->SetPosition(FMath::FRandRange(0.0f, DesiredClip->GetPlayLength()), false);
			bZombieFirstPlayDone = true;
		}
	}
	// Every tick: match playback to actual travel speed (kills foot-sliding within the clamp band).
	if (Node)
	{
		Node->SetPlayRate(AuthoredSpeed > UE_KINDA_SMALL_NUMBER
			? FMath::Clamp(Speed2D / AuthoredSpeed, 0.6f, 1.6f) : 1.0f);
	}
}

bool AEmergeEnemy::StartFallTraversal(const FVector& ObstacleTopPoint, const FVector& LandingPoint)
{
	// Zombie-look only (the ALS body mantles instead), one traversal at a time, never from the air
	// (a scripted lerp starting mid-fall would read as a teleport-glide).
	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!bZombieLookActive || bFallTraversing || !Move || !Move->IsMovingOnGround()) { return false; }

	// Belt-and-suspenders height gate (the controller already gates at MinTraversalHeightUu):
	// curbs/street furniture below ~55uu are step-ups (MaxStepHeight = 50 in SetupZombieLook),
	// never traversals — a scripted topple over a kerb reads as a broken animation loop.
	const float FeetZ = GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	if (ObstacleTopPoint.Z - FeetZ < 55.0f) { return false; }

	// Per-traversal randomization: one clip of each stage + rates + recovery pause, rolled fresh
	// every wall so a horde toppling the same fence doesn't sync up.
	ZombieReachClip = TraversalReachClips.Num() > 0
		? TraversalReachClips[FMath::RandRange(0, TraversalReachClips.Num() - 1)].LoadSynchronous() : nullptr;
	ZombieToppleClip = TraversalToppleClips.Num() > 0
		? TraversalToppleClips[FMath::RandRange(0, TraversalToppleClips.Num() - 1)].LoadSynchronous() : nullptr;
	ZombieRecoverClip = TraversalRecoverClips.Num() > 0
		? TraversalRecoverClips[FMath::RandRange(0, TraversalRecoverClips.Num() - 1)].LoadSynchronous() : nullptr;
	if (!ZombieReachClip || !ZombieToppleClip)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: fall-traversal clips failed to load - traversal refused"), *GetName());
		return false;
	}

	const float HalfH = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FallStart = GetActorLocation();
	FallApex = ObstacleTopPoint + FVector(0.0f, 0.0f, HalfH + 12.0f);   // capsule bottom just clears the lip
	FallLand = LandingPoint + FVector(0.0f, 0.0f, HalfH);
	FallPhase = 0;
	FallPhaseTime = 0.0f;
	FallElapsed = 0.0f;
	FallRecoverPause = FMath::FRandRange(0.4f, 0.9f);

	// Scripted world-space move: MOVE_Flying so the CMC neither applies gravity nor fights the
	// SetActorLocation lerp. Collision stays ON — the lerp sweeps false, but the endpoints were
	// traced clear by the controller.
	Move->StopMovementImmediately();
	Move->SetMovementMode(MOVE_Flying);

	// Rise stage: reach loop = the zombie clawing/dragging itself up the wall face, slowed.
	PlayTraversalClip(ZombieReachClip, true, FMath::FRandRange(0.7f, 0.9f));
	ZombieCurrentClip = nullptr;   // force UpdateZombieAnim to re-issue a loop when we hand back
	bFallTraversing = true;
	++FallTraversalCount;
	return true;
}

void AEmergeEnemy::TickFallTraversal(const float DeltaSeconds)
{
	// Watchdog: never leave a zombie stuck flying — whatever went wrong (destroyed floor, mid-lerp
	// possession swap, bad durations), snap to the landing and hand back to walking.
	FallElapsed += DeltaSeconds;
	if (FallElapsed > 4.0f)
	{
		SetActorLocation(FallLand, false);
		FinishFallTraversal();
		return;
	}

	FallPhaseTime += DeltaSeconds;
	if (FallPhase == 0)
	{
		// Segment 1: drag up and forward to above the obstacle lip — SLOW, a shambler hauling
		// itself up, nothing like a jump.
		const float T = FMath::Clamp(FallPhaseTime / FMath::Max(FallRiseSeconds, 0.1f), 0.0f, 1.0f);
		SetActorLocation(FMath::Lerp(FallStart, FallApex, FMath::SmoothStep(0.0f, 1.0f, T)), false);
		if (T >= 1.0f)
		{
			FallPhase = 1;
			FallPhaseTime = 0.0f;
			// Topple stage: stand-to-crawl collapse played once — falling forward over the edge.
			PlayTraversalClip(ZombieToppleClip, false, FMath::FRandRange(0.7f, 0.9f));
		}
	}
	else if (FallPhase == 1)
	{
		// Segment 2: forward and down to the far-side floor with a slight arc (the body pitches
		// over the lip rather than sliding down a straight ramp line).
		const float T = FMath::Clamp(FallPhaseTime / FMath::Max(FallDropSeconds, 0.1f), 0.0f, 1.0f);
		FVector Pos = FMath::Lerp(FallApex, FallLand, FMath::SmoothStep(0.0f, 1.0f, T));
		Pos.Z += FMath::Sin(T * UE_PI) * 12.0f;
		SetActorLocation(Pos, false);
		if (T >= 1.0f)
		{
			FallPhase = 2;
			FallPhaseTime = 0.0f;
			// Landed: crawl-to-stand scramble while the capsule holds still (shamblers struggle up).
			if (ZombieRecoverClip)
			{
				PlayTraversalClip(ZombieRecoverClip, false, FMath::FRandRange(0.85f, 1.1f));
			}
		}
	}
	else if (FallPhaseTime >= FallRecoverPause)
	{
		FinishFallTraversal();
	}
}

void AEmergeEnemy::FinishFallTraversal()
{
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->SetMovementMode(MOVE_Walking);
		Move->StopMovementImmediately();
	}
	bFallTraversing = false;
	FallPhase = 0;
	// ZombieCurrentClip has been null since StartFallTraversal: UpdateZombieAnim re-issues the
	// right speed-switched loop on its next tick (its self-heal path also covers the non-looping
	// recover clip having finished).
}

void AEmergeEnemy::PlayTraversalClip(UAnimSequence* Clip, const bool bLooping, const float Rate)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp || !Clip) { return; }
	MeshComp->PlayAnimation(Clip, bLooping);
	if (UAnimSingleNodeInstance* Node = MeshComp->GetSingleNodeInstance())
	{
		Node->SetPlayRate(Rate);
	}
}

float AEmergeEnemy::AuthoredSpeedFromRootMotion(const UAnimSequence* IpcClip, const float FallbackSpeed)
{
	if (!IpcClip) { return FallbackSpeed; }
	// Root_Motion sibling = same asset name minus the _IPC suffix.
	FString RootMotionName = IpcClip->GetName();
	RootMotionName.RemoveFromEnd(TEXT("_IPC"));
	const FSoftObjectPath RootMotionPath(
		FString::Printf(TEXT("/Game/Zombie_01/Animation/Root_Motion/%s.%s"), *RootMotionName, *RootMotionName));
	if (const UAnimSequence* RootMotionSeq = Cast<UAnimSequence>(RootMotionPath.TryLoad()))
	{
		const float Length = RootMotionSeq->GetPlayLength();
		if (Length > UE_KINDA_SMALL_NUMBER)
		{
			const FAnimExtractContext ExtractionContext;
			const float Speed = RootMotionSeq->ExtractRootMotionFromRange(0.0f, Length, ExtractionContext)
				.GetTranslation().Size2D() / Length;
			if (Speed >= 10.0f) { return Speed; }   // <10 uu/s = broken/authoring-less extraction
		}
	}
	UE_LOG(LogTemp, Warning,
		TEXT("Zombie clip %s: authored-speed extraction from Root_Motion sibling failed - falling back to %.0f uu/s"),
		*IpcClip->GetName(), FallbackSpeed);
	return FallbackSpeed;
}

UAlsMantlingSettings* AEmergeEnemy::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return (MantlingType == EAlsMantlingType::Low) ? MantlingSettingsLow : MantlingSettingsHigh;
}

bool AEmergeEnemy::IsMantlingAllowedToStart_Implementation() const
{
	// Zombie look = single-node mode: mantling montages need an anim instance we no longer have
	// (flat-ground scenario for the basic-mechanics test) — refuse cleanly so the controller's
	// traversal hop just no-ops instead of Montage_Play-ing into a single-node instance.
	return !bZombieLookActive && Super::IsMantlingAllowedToStart_Implementation();
}

FString AEmergeEnemy::GetAnimDebug() const
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	const UAnimInstance* Anim = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	return FString::Printf(TEXT("{\"anim\":%s,\"gait\":\"%s\",\"action\":\"%s\",\"spd\":%.0f,\"fallTraversing\":%s,\"fallTraversals\":%d}"),
		Anim ? *FString::Printf(TEXT("\"%s\""), *Anim->GetClass()->GetName()) : TEXT("null"),
		*GetDesiredGait().GetTagName().ToString(),
		*GetLocomotionAction().GetTagName().ToString(), GetVelocity().Size2D(),
		bFallTraversing ? TEXT("true") : TEXT("false"), FallTraversalCount);
}
