#include "Framework/EmergeCharacter.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"
#include "Survival/EmergeStaminaComponent.h"
#include "Combat/EmergeDamageComponent.h"
#include "Survival/EmergeStatusEffectComponent.h"
#include "Combat/EmergeEquipmentComponent.h"
#include "Inventory/EmergeInventoryComponent.h"
#include "Combat/EmergeStagger.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Nav/EmergeNavRecovery.h"
#include "Nav/EmergeFlee.h"
#include "Sensor/EmergeWorldSense.h"
#include "AlsCameraComponent.h"
#include "AlsCameraSettings.h"
#include "Settings/AlsMantlingSettings.h"
#include "Animation/AnimMontage.h"
#include "Survival/EmergeSprintGate.h"
#include "Nav/EmergeNavEta.h"
#include "Nav/EmergeNavPathLen.h"
#include "GameFramework/WorldSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Character.h"
#include "HAL/IConsoleManager.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Utility/AlsGameplayTags.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogEmergeTelemetry);

static TAutoConsoleVariable<int32> CVarEmergeTelemetry(
	TEXT("emerge.telemetry"), 0,
	TEXT("Per-tick survivor telemetry to LogEmergeTelemetry (0=off, 1=on). Read back externally via the log."),
	ECVF_Cheat);

AEmergeCharacter::AEmergeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Vitals = CreateDefaultSubobject<UEmergeVitalsComponent>(TEXT("Vitals"));
	Stagger = CreateDefaultSubobject<UEmergeStaggerComponent>(TEXT("Stagger"));
	Stamina = CreateDefaultSubobject<UEmergeStaminaComponent>(TEXT("Stamina"));
	Damage = CreateDefaultSubobject<UEmergeDamageComponent>(TEXT("Damage"));
	StatusEffects = CreateDefaultSubobject<UEmergeStatusEffectComponent>(TEXT("StatusEffects"));
	Equipment = CreateDefaultSubobject<UEmergeEquipmentComponent>(TEXT("Equipment"));
	Inventory = CreateDefaultSubobject<UEmergeInventoryComponent>(TEXT("Inventory"));

	// Reliable 3rd-person camera rig (overrides the unconfigured ALS example camera).
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 70.0f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Navmesh: whole-map dynamic generation (bGenerateNavigationOnlyAroundNavigationInvokers=False
	// in DefaultEngine.ini + the GameMode Build() kick) is the working model — no invoker component;
	// one here would just claim invoker-driven generation the ini flag explicitly disables.

	// ALS gameplay + movement settings (required or the animation refresh early-returns).
	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> AlsSettings(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (AlsSettings.Succeeded()) { Settings = AlsSettings.Object; }
	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> AlsMovement(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (AlsMovement.Succeeded()) { MovementSettings = AlsMovement.Object; }

	// Mantling settings (the ALS example BP assigns these in-editor; without them StartMantling ensures).
	// bAutoCalculateStartTime: the default fixed start-time interpolation skipped most of the montage
	// at ~70uu obstacle heights -> the capsule popped up in <250ms (measured) instead of animating.
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

	// Configure the inherited ALS camera rig (mesh + camera anim + settings) so its BeginPlay
	// ensures pass — these were the two error dialogs at PIE start. The actual view still comes
	// from FollowCamera via our CalcCamera override. Settings is a protected UPROPERTY -> reflection.
	if (Camera)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> CamMesh(
			TEXT("/ALS/ALSCamera/SKM_Als_Camera.SKM_Als_Camera"));
		if (CamMesh.Succeeded()) { Camera->SetSkeletalMesh(CamMesh.Object); }
		static ConstructorHelpers::FClassFinder<UAnimInstance> CamAnim(TEXT("/ALS/ALSCamera/AB_Als_Camera"));
		if (CamAnim.Succeeded()) { Camera->SetAnimInstanceClass(CamAnim.Class); }
		static ConstructorHelpers::FObjectFinder<UAlsCameraSettings> CamSettings(
			TEXT("/ALS/ALSCamera/Data/CS_Als_Default.CS_Als_Default"));
		if (CamSettings.Succeeded())
		{
			if (FObjectProperty* SettingsProp = FindFProperty<FObjectProperty>(
				UAlsCameraComponent::StaticClass(), TEXT("Settings")))
			{
				SettingsProp->SetObjectPropertyValue_InContainer(Camera, CamSettings.Object);
			}
		}

		// Keep the component (its BeginPlay ensures need the assets above) but never let it run:
		// it is a skeletal mesh driving a camera anim BP with AlwaysTickPoseAndRefreshBones — a
		// whole second rig evaluating every frame for a view we never use (CalcCamera always takes
		// FollowCamera). It auto-activates on register (USkinnedMeshComponent), which is what turns
		// its tick on, so block that at the source. Inactive is safe: both our CalcCamera override
		// and AAlsCharacterExample's (which gates on Camera->IsActive()) route around it.
		Camera->SetAutoActivate(false);
		Camera->Deactivate();
		Camera->SetComponentTickEnabled(false);
	}

	// Enhanced Input: assign the ALS mapping context + action assets (inherited protected members).
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(
		TEXT("/ALS/ALS/Data/Input/IMC_Als_Default.IMC_Als_Default"));
	if (IMC.Succeeded()) { InputMappingContext = IMC.Object; }

#define EMERGE_FIND_IA(Member, Path) \
	{ static ConstructorHelpers::FObjectFinder<UInputAction> Finder_##Member(TEXT(Path)); \
	  if (Finder_##Member.Succeeded()) { Member = Finder_##Member.Object; } }

	EMERGE_FIND_IA(LookMouseAction,     "/ALS/ALS/Data/Input/IA_Als_LookMouse.IA_Als_LookMouse")
	EMERGE_FIND_IA(LookAction,          "/ALS/ALS/Data/Input/IA_Als_Look.IA_Als_Look")
	EMERGE_FIND_IA(MoveAction,          "/ALS/ALS/Data/Input/IA_Als_Move.IA_Als_Move")
	EMERGE_FIND_IA(SprintAction,        "/ALS/ALS/Data/Input/IA_Als_Sprint.IA_Als_Sprint")
	EMERGE_FIND_IA(WalkAction,          "/ALS/ALS/Data/Input/IA_Als_Walk.IA_Als_Walk")
	EMERGE_FIND_IA(CrouchAction,        "/ALS/ALS/Data/Input/IA_Als_Crouch.IA_Als_Crouch")
	EMERGE_FIND_IA(JumpAction,          "/ALS/ALS/Data/Input/IA_Als_Jump.IA_Als_Jump")
	EMERGE_FIND_IA(AimAction,           "/ALS/ALS/Data/Input/IA_Als_Aim.IA_Als_Aim")
	EMERGE_FIND_IA(RagdollAction,       "/ALS/ALS/Data/Input/IA_Als_Ragdoll.IA_Als_Ragdoll")
	EMERGE_FIND_IA(RollAction,          "/ALS/ALS/Data/Input/IA_Als_Roll.IA_Als_Roll")
	EMERGE_FIND_IA(RotationModeAction,  "/ALS/ALS/Data/Input/IA_Als_RotationMode.IA_Als_RotationMode")
	EMERGE_FIND_IA(ViewModeAction,      "/ALS/ALS/Data/Input/IA_Als_ViewMode.IA_Als_ViewMode")
	EMERGE_FIND_IA(SwitchShoulderAction,"/ALS/ALS/Data/Input/IA_Als_SwitchShoulder.IA_Als_SwitchShoulder")

#undef EMERGE_FIND_IA
}

void AEmergeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);   // ALS example: camera + input-driven locomotion

	// Sprint stamina economy: drain while actually sprinting (heavier load drains faster), regen
	// otherwise; hitting 0 locks sprint until stamina re-arms (gate kernel = hysteresis, no flapping).
	if (Stamina)
	{
		const bool bSprintingNow = GetGait() == AlsGaitTags::Sprinting && GetVelocity().Size2D() > 10.0f;
		Stamina->Simulate(DeltaSeconds, bSprintingNow, Inventory ? Inventory->GetLoadTier() : 0);
		bSprintExhausted = UEmergeSprintGate::UpdateExhausted(Stamina->Stamina, bSprintExhausted, SprintReArmStamina);
		if (bSprintExhausted && GetDesiredGait() == AlsGaitTags::Sprinting)
		{
			SetDesiredGait(AlsGaitTags::Running);   // covers both key-held and programmatic sprint
		}
	}

	// Mismatch/glitch timers (proprioception vs outcome).
	if (UCharacterMovementComponent* MC = GetCharacterMovement())
	{
		const bool bGnd = MC->IsMovingOnGround();
		const bool bIn = MC->GetLastInputVector().SizeSquared() > 0.01f;
		StuckTime = (bIn && GetVelocity().Size2D() < 10.0f && bGnd) ? StuckTime + DeltaSeconds : 0.0f;
		AirTime = bGnd ? 0.0f : AirTime + DeltaSeconds;
	}

	// Continuous evasion: replan-and-flee loop feeding the nav-follower below with fresh goals.
	if (bEvading)
	{
		TickEvade(DeltaSeconds);
	}

	// nav-follow: steer toward the current path waypoint (manual follower, drives ALS via AddMovementInput).
	if (bNavigating && NavPath.IsValidIndex(NavIdx))
	{
		const FVector Cur = GetActorLocation();
		if (FVector::Dist2D(Cur, NavPath[NavIdx]) <= 120.0f)
		{
			++NavIdx;
			if (!NavPath.IsValidIndex(NavIdx)) { bNavigating = false; NavState = TEXT("arrived"); RestoreNavFacing(); }
		}
		else
		{
			const FVector WP = NavPath[NavIdx];
			const FVector2D Heading = (FVector2D(WP.X, WP.Y) - FVector2D(Cur.X, Cur.Y)).GetSafeNormal();
			const FVector Fwd = GetActorForwardVector();
			const FVector2D Facing = FVector2D(Fwd.X, Fwd.Y).GetSafeNormal();
			NavTurnErrorDeg = (Heading.IsNearlyZero() || Facing.IsNearlyZero()) ? 0.0f
				: FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector2D::DotProduct(Heading, Facing), -1.0f, 1.0f)));
			// Turn-gate: at a sharp corner, throttle down so he pivots to face the new segment before running down it.
			const float Throttle = (NavTurnErrorDeg > 50.0f) ? 0.15f : 1.0f;
			AddMovementInput(FVector(Heading.X, Heading.Y, 0.0f), Throttle);
			// Progress tracking (path-length remaining) + stuck-recovery ladder (continue -> repath -> abandon).
			const float CurDist = UEmergeNavPathLen::RemainingLength(NavPath, NavIdx, Cur);
			bNavMakingProgress = (NavLastDist < 0.0f) ? true : UEmergeNavEta::MakingProgress(NavLastDist, CurDist, 5.0f);
			NavLastDist = CurDist;
			NavStuckTime = (GetVelocity().Size2D() < 10.0f) ? NavStuckTime + DeltaSeconds : 0.0f;
			// PROACTIVE vault: low obstacle directly ahead while moving -> mantle before any stall
			// (Dying Light flow). Knee trace blocked + chest trace clear = vaultable band.
			if (GetLocomotionAction() != AlsLocomotionActionTags::Mantling && GetVelocity().Size2D() > 150.0f
				&& GetCharacterMovement() && GetCharacterMovement()->IsMovingOnGround())
			{
				const float Look = FMath::Clamp(GetVelocity().Size2D() * 0.35f, 120.0f, 250.0f);
				const FVector Feet = GetActorLocation()
					- FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
				const FVector Ahead(Heading.X, Heading.Y, 0.0f);
				FCollisionQueryParams VaultQP(SCENE_QUERY_STAT(EmergeVaultProbe), false, this);
				FHitResult LowHit, HighHit;
				bool bLowBlocked = GetWorld()->LineTraceSingleByChannel(LowHit,
					Feet + FVector(0, 0, 45.0f), Feet + FVector(0, 0, 45.0f) + Ahead * Look, ECC_Visibility, VaultQP);
				const bool bHighBlocked = GetWorld()->LineTraceSingleByChannel(HighHit,
					Feet + FVector(0, 0, 140.0f), Feet + FVector(0, 0, 140.0f) + Ahead * (Look + 50.0f), ECC_Visibility, VaultQP);
				// Walkable hit = ramp/stair, never a vault target (canonical slope guard).
				if (bLowBlocked && GetCharacterMovement()->IsWalkable(LowHit)) { bLowBlocked = false; }
				if (bLowBlocked && !bHighBlocked && IsMantlingAllowedToStart() && StartMantling())
				{
					++NavVaultCount;
					NavStuckTime = 0.0f;
				}
			}
			// REACTIVE fallback: briefly stuck against something the probes missed -> try a mantle
			// before the repath ladder (ALS validates the surface itself). Cooldown between attempts:
			// while pinned against an unmantleable surface this branch is true EVERY tick, and each
			// StartMantling attempt runs the full ALS mantle trace suite for nothing.
			if (NavStuckTime > 0.5f && NavStuckTime <= 3.0f && GetLocomotionAction() != AlsLocomotionActionTags::Mantling
				&& GetWorld()->GetTimeSeconds() >= NextReactiveVaultTime)
			{
				NextReactiveVaultTime = GetWorld()->GetTimeSeconds() + 0.35f;
				if (IsMantlingAllowedToStart() && StartMantling())
				{
					++NavVaultCount;
					NavStuckTime = 0.0f;
				}
			}
			if (NavStuckTime > 3.0f)
			{
				const int32 Action = UEmergeNavRecovery::DecideRecovery(NavStuckTime, NavRepathCount, 3.0f, 2);
				if (Action == 1) { ++NavRepathCount; NavState = TEXT("repathing"); ComputePathTo(NavGoal); if (!bNavigating) { RestoreNavFacing(); } }
				else if (UEmergeNavRecovery::ShouldGiveUp(Action)) { bNavigating = false; NavState = TEXT("blocked"); RestoreNavFacing(); }
			}
		}
	}

	if (CVarEmergeTelemetry.GetValueOnGameThread() != 0)
	{
		const UCharacterMovementComponent* Move = GetCharacterMovement();
		const FVector Vel = Move ? Move->Velocity : FVector::ZeroVector;
		const FVector Input = Move ? Move->GetLastInputVector() : FVector::ZeroVector;
		const FRotator Ctrl = GetControlRotation();
		UE_LOG(LogEmergeTelemetry, Log,
			TEXT("EMERGE_TEL spd=%.1f vel=%s input=%s ctrl=%s gait=%s stance=%s rotmode=%s overlay=%s locmode=%s action=%s"),
			Vel.Size2D(), *Vel.ToCompactString(), *Input.ToCompactString(), *Ctrl.ToCompactString(),
			*GetGait().GetTagName().ToString(), *GetStance().GetTagName().ToString(),
			*GetRotationMode().GetTagName().ToString(), *GetOverlayMode().GetTagName().ToString(),
			*GetLocomotionMode().GetTagName().ToString(), *GetLocomotionAction().GetTagName().ToString());
	}

	// Stagger differentiator: a knockdown drops the survivor into a physics ragdoll.
	if (Stagger)
	{
		const EEmergeStaggerState State = Stagger->CurrentState();
		if ((State == EEmergeStaggerState::Knockdown || State == EEmergeStaggerState::Dead) &&
			GetLocomotionAction() != AlsLocomotionActionTags::Ragdolling)
		{
			StartRagdolling();
		}
	}

	// Bleeding drains vitals continuously.
	if (StatusEffects && Vitals)
	{
		StatusEffects->Simulate(DeltaSeconds, Vitals);
	}
}



void AEmergeCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	// Use our 3rd-person rig instead of the ALS example camera (which our reparent left unconfigured).
	if (FollowCamera)
	{
		FollowCamera->GetCameraView(DeltaTime, OutResult);
		return;
	}
	Super::CalcCamera(DeltaTime, OutResult);
}

FString AEmergeCharacter::SenseWorld()
{
	UEmergeWorldSense* WorldSense = GetWorld() ? GetWorld()->GetSubsystem<UEmergeWorldSense>() : nullptr;
	return WorldSense ? WorldSense->SenseWorld() : TEXT("{\"error\":\"no world sense\"}");
}

FString AEmergeCharacter::SenseSkeleton(const FString& NameFilter)
{
	UEmergeWorldSense* WorldSense = GetWorld() ? GetWorld()->GetSubsystem<UEmergeWorldSense>() : nullptr;
	return WorldSense ? WorldSense->SenseSkeleton(NameFilter) : TEXT("{\"error\":\"no world sense\"}");
}

FString AEmergeCharacter::SenseEnvironment(float Radius)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return TEXT("{\"error\":\"no world\"}");
	}

	const FVector Pos = GetActorLocation();
	const FVector Vel = GetVelocity();
	const FRotator ActorRot = GetActorRotation();
	const FVector Fwd = GetActorForwardVector();
	const FVector Origin = Pos + FVector(0.0f, 0.0f, 40.0f);
	UCharacterMovementComponent* Move = GetCharacterMovement();
	FCollisionQueryParams TP;
	TP.AddIgnoredActor(this);

	const bool bGrounded = Move && Move->IsMovingOnGround();
	const bool bFalling = Move && Move->IsFalling();
	const FVector InputVec = Move ? Move->GetLastInputVector() : FVector::ZeroVector;
	const bool bHasInput = InputVec.SizeSquared() > 0.01f;
	const float Spd = Vel.Size2D();
	FString MoveMode = TEXT("none");
	if (Move)
	{
		switch (Move->MovementMode)
		{
		case MOVE_Walking: MoveMode = TEXT("walking"); break;
		case MOVE_NavWalking: MoveMode = TEXT("navwalking"); break;
		case MOVE_Falling: MoveMode = TEXT("falling"); break;
		case MOVE_Swimming: MoveMode = TEXT("swimming"); break;
		case MOVE_Flying: MoveMode = TEXT("flying"); break;
		default: MoveMode = TEXT("other"); break;
		}
	}
	const FString SelfJson = FString::Printf(
		TEXT("{\"pos\":[%.0f,%.0f,%.0f],\"vel\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"velZ\":%.0f,\"move\":\"%s\",\"grounded\":%s,\"falling\":%s,\"hasInput\":%s}"),
		Pos.X, Pos.Y, Pos.Z, Vel.X, Vel.Y, Vel.Z, Spd, Vel.Z, *MoveMode,
		bGrounded ? TEXT("true") : TEXT("false"), bFalling ? TEXT("true") : TEXT("false"), bHasInput ? TEXT("true") : TEXT("false"));

	FVector CamLoc = Origin, CamFwd = Fwd;
	float FOV = 90.0f;
	if (FollowCamera) { CamLoc = FollowCamera->GetComponentLocation(); CamFwd = FollowCamera->GetForwardVector(); FOV = FollowCamera->FieldOfView; }
	const FString CamJson = FString::Printf(TEXT("{\"loc\":[%.0f,%.0f,%.0f],\"fwd\":[%.2f,%.2f,%.2f],\"fov\":%.0f}"),
		CamLoc.X, CamLoc.Y, CamLoc.Z, CamFwd.X, CamFwd.Y, CamFwd.Z, FOV);

	FString AimJson = TEXT("null");
	{
		FHitResult AimHit;
		if (World->LineTraceSingleByChannel(AimHit, CamLoc, CamLoc + CamFwd * 15000.0f, ECC_Visibility, TP) && AimHit.GetActor())
		{
			AimJson = FString::Printf(TEXT("{\"hit\":\"%s\",\"cls\":\"%s\",\"d\":%.0f,\"pt\":[%.0f,%.0f,%.0f]}"),
				*AimHit.GetActor()->GetName(), *AimHit.GetActor()->GetClass()->GetName(), AimHit.Distance,
				AimHit.ImpactPoint.X, AimHit.ImpactPoint.Y, AimHit.ImpactPoint.Z);
		}
	}

	auto CellKey = [](float X, float Y) -> int64
	{
		const int32 cx = FMath::FloorToInt(X / 200.0f);
		const int32 cy = FMath::FloorToInt(Y / 200.0f);
		return ((int64)cx << 32) | (int64)((uint32)cy);
	};
	// Bound world-memory growth (test-session scale): a long soak would otherwise accrete cells forever.
	if (OccGrid.Num() > 4096) { OccGrid.Empty(1024); }
	OccGrid.Add(CellKey(Pos.X, Pos.Y), 1);

	TArray<FString> Rays;
	const int32 N = 16;
	const float RayLen = 5000.0f;
	for (int32 i = 0; i < N; ++i)
	{
		const float Ang = (360.0f / N) * i;
		const FVector Dir = ActorRot.RotateVector(FRotator(0.0f, Ang, 0.0f).RotateVector(FVector::ForwardVector));
		FHitResult Hit;
		const bool bHit = World->LineTraceSingleByChannel(Hit, Origin, Origin + Dir * RayLen, ECC_Visibility, TP);
		FString Cat = TEXT("open");
		float NormD = 1.0f, Nz = 0.0f;
		FString HitName = TEXT("none");
		if (bHit)
		{
			NormD = Hit.Distance / RayLen;
			Nz = Hit.ImpactNormal.Z;
			AActor* HA = Hit.GetActor();
			HitName = HA ? HA->GetName() : TEXT("?");
			if (HA && HA->IsA(APawn::StaticClass())) Cat = TEXT("character");
			else if (HitName.Contains(TEXT("build")) || HitName.Contains(TEXT("wall")) || HitName.Contains(TEXT("pillar")) || HitName.Contains(TEXT("storefront"))) Cat = TEXT("building");
			else if (HitName.Contains(TEXT("ground")) || HitName.Contains(TEXT("road")) || HitName.Contains(TEXT("walkway")) || HitName.Contains(TEXT("curb"))) Cat = TEXT("ground");
			else Cat = TEXT("prop");
			OccGrid.Add(CellKey(Hit.ImpactPoint.X, Hit.ImpactPoint.Y), 2);
			const int32 Steps = FMath::Min(12, (int32)(Hit.Distance / 200.0f));
			for (int32 s = 1; s < Steps; ++s)
			{
				const FVector SP = Origin + Dir * (200.0f * s);
				uint8& Cell = OccGrid.FindOrAdd(CellKey(SP.X, SP.Y));
				if (Cell != 2) Cell = 1;
			}
		}
		Rays.Add(FString::Printf(TEXT("{\"a\":%.0f,\"nd\":%.2f,\"cat\":\"%s\",\"nz\":%.2f,\"hit\":\"%s\"}"), Ang, NormD, *Cat, Nz, *HitName));
	}

	TArray<FString> Ledges;
	float CenterDrop = 999.0f, CenterWallD = -1.0f;
	bool bCenterWall = false, bCenterLedge = false;
	const float AheadDist = 150.0f;
	for (int32 k = -1; k <= 1; ++k)
	{
		const float Ang = 20.0f * k;
		const FVector Dir = ActorRot.RotateVector(FRotator(0.0f, Ang, 0.0f).RotateVector(FVector::ForwardVector));
		const FVector ProbeTop = Pos + Dir * AheadDist + FVector(0.0f, 0.0f, 60.0f);
		FHitResult FloorAhead;
		const bool bFA = World->LineTraceSingleByChannel(FloorAhead, ProbeTop, ProbeTop - FVector(0.0f, 0.0f, 400.0f), ECC_Visibility, TP);
		FHitResult WallAhead;
		const FVector Eye = Pos + FVector(0.0f, 0.0f, 40.0f);
		const bool bW = World->LineTraceSingleByChannel(WallAhead, Eye, Eye + Dir * AheadDist, ECC_Visibility, TP);
		const float Drop = bFA ? (ProbeTop.Z - FloorAhead.ImpactPoint.Z) : 999.0f;
		if (k == 0) { CenterDrop = Drop; bCenterWall = bW; CenterWallD = bW ? WallAhead.Distance : -1.0f; bCenterLedge = !bFA; }
		Ledges.Add(FString::Printf(TEXT("{\"a\":%.0f,\"floor\":%s,\"drop\":%.0f,\"ledge\":%s,\"wall\":%s,\"wall_d\":%.0f}"),
			Ang, bFA ? TEXT("true") : TEXT("false"), Drop, (!bFA) ? TEXT("true") : TEXT("false"), bW ? TEXT("true") : TEXT("false"), bW ? WallAhead.Distance : -1.0f));
	}

	FHitResult FloorHit;
	const bool bFloor = World->LineTraceSingleByChannel(FloorHit, Origin, Origin - FVector(0, 0, 5000.0f), ECC_Visibility, TP);
	FHitResult CeilHit;
	const bool bCeil = World->LineTraceSingleByChannel(CeilHit, Origin, Origin + FVector(0, 0, 5000.0f), ECC_Visibility, TP);
	const float FloorD = bFloor ? FloorHit.Distance : -1.0f;
	const float CeilD = bCeil ? CeilHit.Distance : -1.0f;

	const float KillZ = World->GetWorldSettings() ? World->GetWorldSettings()->KillZ : -1.0e9f;
	const bool bBelowKill = Pos.Z < KillZ;
	const bool bFellThrough = bFalling && Vel.Z < -50.0f && !bFloor;
	const FString AnomJson = FString::Printf(TEXT("{\"fellThroughFloor\":%s,\"belowKillZ\":%s}"),
		bFellThrough ? TEXT("true") : TEXT("false"), bBelowKill ? TEXT("true") : TEXT("false"));

	const bool bCanForward = !bCenterWall && !bCenterLedge && CenterDrop < 60.0f;
	const bool bCanClimb = bCenterWall && CenterWallD >= 0.0f && CenterWallD < 120.0f;
	const bool bNearLedge = bCenterLedge || CenterDrop > 120.0f;
	const FString AffJson = FString::Printf(TEXT("{\"canMoveForward\":%s,\"canClimb\":%s,\"nearLedge\":%s}"),
		bCanForward ? TEXT("true") : TEXT("false"), bCanClimb ? TEXT("true") : TEXT("false"), bNearLedge ? TEXT("true") : TEXT("false"));

	const bool bPressedNotMoving = bHasInput && Spd < 10.0f && bGrounded;
	const FString MismatchJson = FString::Printf(TEXT("{\"stuckTime\":%.1f,\"airTime\":%.1f,\"pressedNotMoving\":%s,\"groundedAndFalling\":%s}"),
		StuckTime, AirTime, bPressedNotMoving ? TEXT("true") : TEXT("false"), (bGrounded && bFalling) ? TEXT("true") : TEXT("false"));

	FString ConseqVerdict = TEXT("clear");
	if (bCenterWall && CenterWallD >= 0.0f && CenterWallD < 100.0f) ConseqVerdict = TEXT("blocked");
	else if (CenterDrop > 200.0f || bCenterLedge) ConseqVerdict = TEXT("dropoff");
	const FString ConseqJson = FString::Printf(TEXT("{\"moveForward\":\"%s\",\"dropAhead\":%.0f,\"wallAhead\":%.0f}"),
		*ConseqVerdict, CenterDrop, CenterWallD);

	const FString InvJson = FString::Printf(TEXT("{\"posNaN\":%s,\"velNaN\":%s,\"speedSane\":%s,\"stuckTooLong\":%s}"),
		Pos.ContainsNaN() ? TEXT("true") : TEXT("false"), Vel.ContainsNaN() ? TEXT("true") : TEXT("false"),
		(Spd < 5000.0f) ? TEXT("true") : TEXT("false"), (StuckTime > 3.0f) ? TEXT("true") : TEXT("false"));

	int32 FreeCells = 0, ObsCells = 0, FrontierCount = 0;
	FVector NearestFrontier = FVector::ZeroVector;
	float NF = 1.0e9f;
	for (const TPair<int64, uint8>& KV : OccGrid)
	{
		if (KV.Value == 2) { ObsCells++; continue; }
		FreeCells++;
		const int32 cx = (int32)(KV.Key >> 32);
		const int32 cy = (int32)(KV.Key & 0xFFFFFFFF);
		const int32 dxs[4] = { 1, -1, 0, 0 };
		const int32 dys[4] = { 0, 0, 1, -1 };
		bool bFront = false;
		for (int32 d = 0; d < 4; ++d)
		{
			const int64 nk = ((int64)(cx + dxs[d]) << 32) | (int64)((uint32)(cy + dys[d]));
			if (!OccGrid.Contains(nk)) { bFront = true; break; }
		}
		if (bFront)
		{
			FrontierCount++;
			const FVector CW((cx + 0.5f) * 200.0f, (cy + 0.5f) * 200.0f, Pos.Z);
			const float dd = FVector::Dist2D(CW, Pos);
			if (dd < NF) { NF = dd; NearestFrontier = CW; }
		}
	}
	const FVector FRel = NearestFrontier - Pos;
	const FString MemJson = FString::Printf(TEXT("{\"knownCells\":%d,\"freeCells\":%d,\"obstacleCells\":%d,\"frontiers\":%d,\"nearestFrontierRel\":[%.0f,%.0f],\"nearestFrontierDist\":%.0f}"),
		OccGrid.Num(), FreeCells, ObsCells, FrontierCount, FRel.X, FRel.Y, (FrontierCount > 0) ? NF : -1.0f);

	FString BonesJson = TEXT("null");
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		auto Rel = [&](const TCHAR* B) -> FString
		{
			const FVector R = SkelMesh->GetSocketLocation(FName(B)) - Pos;
			return FString::Printf(TEXT("[%.0f,%.0f,%.0f]"), R.X, R.Y, R.Z);
		};
		const FVector HandL = SkelMesh->GetSocketLocation(FName(TEXT("hand_l")));
		const FVector HandR = SkelMesh->GetSocketLocation(FName(TEXT("hand_r")));
		const float HandSpread = FVector::Dist2D(HandL, HandR);
		// Bone validity: probe the skeleton itself — GetSocketLocation on a missing bone returns the
		// component location (NOT near-zero), so the old !IsNearlyZero heuristic never caught a
		// wrong-skeleton swap.
		const bool bBonesValid = SkelMesh->DoesSocketExist(FName(TEXT("head")))
			&& SkelMesh->DoesSocketExist(FName(TEXT("pelvis")));
		BonesJson = FString::Printf(
			TEXT("{\"head\":%s,\"hand_l\":%s,\"hand_r\":%s,\"foot_l\":%s,\"foot_r\":%s,\"pelvis\":%s,\"handSpread\":%.0f,\"tposeSuspect\":%s,\"valid\":%s}"),
			*Rel(TEXT("head")), *Rel(TEXT("hand_l")), *Rel(TEXT("hand_r")), *Rel(TEXT("foot_l")), *Rel(TEXT("foot_r")), *Rel(TEXT("pelvis")),
			HandSpread, (HandSpread > 120.0f) ? TEXT("true") : TEXT("false"), bBonesValid ? TEXT("true") : TEXT("false"));
	}

	FString AnimJson;
	{
		const FString Action = GetLocomotionAction().IsValid() ? GetLocomotionAction().GetTagName().ToString() : FString(TEXT("none"));
		FString Montage = TEXT("none");
		float MPos = -1.0f;
		if (USkeletalMeshComponent* Mesh2 = GetMesh())
		{
			if (UAnimInstance* AI = Mesh2->GetAnimInstance())
			{
				if (UAnimMontage* M = AI->GetCurrentActiveMontage()) { Montage = M->GetName(); MPos = AI->Montage_GetPosition(M); }
			}
		}
		AnimJson = FString::Printf(TEXT("{\"action\":\"%s\",\"gait\":\"%s\",\"stance\":\"%s\",\"rotmode\":\"%s\",\"overlay\":\"%s\",\"montage\":\"%s\",\"montagePos\":%.2f}"),
			*Action, *GetGait().GetTagName().ToString(), *GetStance().GetTagName().ToString(), *GetRotationMode().GetTagName().ToString(), *GetOverlayMode().GetTagName().ToString(), *Montage, MPos);
	}

	struct FNear { float Dist; FString Line; };
	TArray<FNear> Near;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* A = *It;
		if (!A || A == this) continue;
		if (!(A->IsA(ACharacter::StaticClass()) || A->IsA(APawn::StaticClass()) || A->IsA(AStaticMeshActor::StaticClass()))) continue;
		const FVector RelP = A->GetActorLocation() - Pos;
		const float Dist = RelP.Size();
		if (Dist > Radius) continue;
		Near.Add({ Dist, FString::Printf(TEXT("{\"name\":\"%s\",\"cls\":\"%s\",\"rel\":[%.0f,%.0f,%.0f],\"d\":%.0f}"),
			*A->GetName(), *A->GetClass()->GetName(), RelP.X, RelP.Y, RelP.Z, Dist) });
	}
	Near.Sort([](const FNear& L, const FNear& R) { return L.Dist < R.Dist; });
	TArray<FString> ActorLines;
	for (int32 i = 0; i < Near.Num() && i < 20; ++i) ActorLines.Add(Near[i].Line);

	return FString::Printf(
		TEXT("{\"self\":%s,\"cam\":%s,\"aim\":%s,\"floor_d\":%.0f,\"ceil_d\":%.0f,")
		TEXT("\"rays\":[%s],\"ledges\":[%s],\"anomalies\":%s,\"affordances\":%s,")
		TEXT("\"mismatch\":%s,\"consequence\":%s,\"invariants\":%s,\"memory\":%s,")
		TEXT("\"bones\":%s,\"anim\":%s,\"actors\":[%s]}"),
		*SelfJson, *CamJson, *AimJson, FloorD, CeilD,
		*FString::Join(Rays, TEXT(",")), *FString::Join(Ledges, TEXT(",")), *AnomJson, *AffJson,
		*MismatchJson, *ConseqJson, *InvJson, *MemJson,
		*BonesJson, *AnimJson, *FString::Join(ActorLines, TEXT(",")));
}

bool AEmergeCharacter::ComputePathTo(FVector Destination)
{
	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!Nav) { NavState = TEXT("no_navsystem"); bNavigating = false; return false; }
	UNavigationPath* Path = Nav->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), Destination, this);
	if (!Path || !Path->IsValid() || Path->PathPoints.Num() < 2)
	{
		NavState = TEXT("no_path"); bNavigating = false; return false;
	}
	NavPath = Path->PathPoints;
	NavIdx = 1;
	NavGoal = Destination;
	NavStuckTime = 0.0f;
	NavLastDist = -1.0f;
	bNavigating = true;
	NavState = Path->IsPartial() ? TEXT("following_partial") : TEXT("following");
	return true;
}

bool AEmergeCharacter::NavigateTo(FVector Destination)
{
	NavRepathCount = 0;
	const bool bOk = ComputePathTo(Destination);
	if (bOk)
	{
		if (!bNavRotationOverridden) { PrevRotationMode = GetDesiredRotationMode(); bNavRotationOverridden = true; }
		SetDesiredRotationMode(AlsRotationModeTags::VelocityDirection);
	}
	return bOk;
}

void AEmergeCharacter::StopNavigating()
{
	bNavigating = false;
	NavState = TEXT("idle");
	NavPath.Reset();
	NavIdx = 0;
	RestoreNavFacing();
}

FString AEmergeCharacter::GetNavProgress()
{
	const FVector Cur = GetActorLocation();
	const float DistRem = bNavigating ? FVector::Dist2D(Cur, NavGoal) : 0.0f;
	const float PathRem = bNavigating ? UEmergeNavPathLen::RemainingLength(NavPath, NavIdx, Cur) : 0.0f;
	const float Eta = UEmergeNavEta::EtaSeconds(PathRem, GetVelocity().Size2D());
	return FString::Printf(
		TEXT("{\"state\":\"%s\",\"navigating\":%s,\"waypoint\":%d,\"total\":%d,\"distRemaining\":%.0f,\"pathRemaining\":%.0f,\"etaSeconds\":%.1f,\"makingProgress\":%s,\"repaths\":%d,\"vaults\":%d,\"stuckTime\":%.1f,\"turnErrDeg\":%.0f,\"stamina\":%.0f,\"sprintExhausted\":%s,\"evading\":%s,\"threatDist\":%d,\"evadeScore\":%d,\"cornered\":%s,\"patrol\":%s,\"cp\":%d}"),
		*NavState, bNavigating ? TEXT("true") : TEXT("false"), NavIdx, NavPath.Num(), DistRem, PathRem, Eta,
		bNavMakingProgress ? TEXT("true") : TEXT("false"), NavRepathCount, NavVaultCount, NavStuckTime, NavTurnErrorDeg,
		Stamina ? Stamina->Stamina : -1.0f, bSprintExhausted ? TEXT("true") : TEXT("false"),
		bEvading ? TEXT("true") : TEXT("false"), (int32)EvadeThreatDist, (int32)EvadeGoalScore,
		bEvadeCornered ? TEXT("true") : TEXT("false"), bPatrolling ? TEXT("true") : TEXT("false"), PatrolIdx);
}

void AEmergeCharacter::RestoreNavFacing()
{
	if (bNavRotationOverridden)
	{
		SetDesiredRotationMode(PrevRotationMode);
		bNavRotationOverridden = false;
	}
	if (bFleeing)
	{
		SetSprinting(false);
		bFleeing = false;
	}
}

UAlsMantlingSettings* AEmergeCharacter::SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType)
{
	return (MantlingType == EAlsMantlingType::Low) ? MantlingSettingsLow : MantlingSettingsHigh;
}

void AEmergeCharacter::SetSprinting(bool bSprint)
{
	// While the patrol walk-gait override is engaged, "not sprinting" means WALKING, not Running —
	// otherwise any incidental SetSprinting(false) (flee teardown, etc.) silently breaks the pacing.
	const FGameplayTag NonSprintGait = bPatrolGaitOverridden ? AlsGaitTags::Walking : AlsGaitTags::Running;
	SetDesiredGait((bSprint && !bSprintExhausted) ? AlsGaitTags::Sprinting : NonSprintGait);
}

bool AEmergeCharacter::FleeFrom(FVector ThreatPos)
{
	UWorld* World = GetWorld();
	UNavigationSystemV1* Nav = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;
	if (!Nav) { return false; }

	const FVector Self = GetActorLocation();
	// Flee in the ground plane; a threat at/above our own position (degenerate) must not produce a
	// vertical away-vector (measured: candidates projected 6000uu into the sky -> all rejected).
	FVector Away = UEmergeFlee::FleeDirection(Self, ThreatPos);
	Away.Z = 0.0f;
	Away = Away.GetSafeNormal();
	if (Away.IsNearlyZero())
	{
		Away = -GetActorForwardVector();
		Away.Z = 0.0f;
		Away = Away.GetSafeNormal();
		if (Away.IsNearlyZero()) { return false; }
	}

	// Fan of candidates around the pure away-direction; score = escape distance actually gained
	// minus a detour penalty from the real path length (a straight-line pick could route back
	// past the threat). Only reachable, complete paths qualify.
	float BestScore = -FLT_MAX;
	FVector BestGoal = FVector::ZeroVector;
	bool bFound = false;
	// Shrinking rings: near the navmesh edge every long candidate projects outside the mesh
	// (measured at the map border), so fall back to closer rings before giving up.
	static constexpr float FleeDists[] = { 6000.0f, 3000.0f, 1500.0f };
	for (const float FleeDist : FleeDists)
	{
		for (int32 Step = -3; Step <= 3; ++Step)
		{
			const FVector Dir = Away.RotateAngleAxis(Step * 25.0f, FVector::UpVector);
			FNavLocation Projected;
			if (!Nav->ProjectPointToNavigation(Self + Dir * FleeDist, Projected, FVector(500.0f, 500.0f, 1000.0f)))
			{
				continue;
			}
			UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, Self, Projected.Location);
			if (!Path || !Path->IsValid() || Path->IsPartial()) { continue; }
			const float EscapeGain = FVector::Dist2D(Projected.Location, ThreatPos) - FVector::Dist2D(Self, ThreatPos);
			const float Score = UEmergeFlee::ScoreFleeCandidate(
				EscapeGain, Path->GetPathLength(), FVector::Dist2D(Self, Projected.Location), 1500.0f);
			if (Score > BestScore) { BestScore = Score; BestGoal = Projected.Location; bFound = true; }
		}
		if (bFound) { break; }
	}
	if (!bFound) { return false; }
	if (!NavigateTo(BestGoal)) { return false; }
	bFleeing = true;
	SetSprinting(true);
	return true;
}

void AEmergeCharacter::StartEvading(AActor* Threat)
{
	if (!Threat) { return; }
	EvadeThreat = Threat;
	bEvading = true;
	bEvadeWasInDanger = false;
	bEvadeCornered = false;
	EvadeGoal = FVector::ZeroVector;
	EvadeGoalScore = 0.0f;
	EvadeReplanTimer = 0.0f;      // replan on the very next tick
	EvadeBiasRerollTime = 0.0f;   // roll a fresh bias immediately
	// Running gait only ("slow jog"): do NOT engage the sprint/stamina system while merely
	// evading in the comfort band — sprint stays in reserve for a real emergency.
	SetSprinting(false);
}

void AEmergeCharacter::StopEvading()
{
	// Restore the pre-patrol gait even on a direct StopEvading (StopPatrolEvade routes here too).
	if (bPatrolGaitOverridden)
	{
		bPatrolGaitOverridden = false;
		SetDesiredGait(PrePatrolDesiredGait.IsValid() ? PrePatrolDesiredGait : AlsGaitTags::Running);
	}
	if (!bEvading) { return; }
	bEvading = false;
	EvadeThreat = nullptr;
	EvadeThreatDist = -1.0f;
	bEvadeCornered = false;
	StopNavigating();
}

void AEmergeCharacter::StartPatrolEvade(const TArray<FVector>& Checkpoints, AActor* Threat)
{
	if (Checkpoints.Num() == 0 || !Threat) { return; }
	PatrolPoints = Checkpoints;
	PatrolIdx = 0;
	bPatrolling = true;
	bPatrolLegActive = false;   // TickEvade's comfort branch issues the first leg
	const FGameplayTag GaitBefore = GetDesiredGait();   // capture BEFORE StartEvading's SetSprinting(false) forces Running
	StartEvading(Threat);
	if (bPatrolWalkGait)
	{
		// Walking for the whole patrol-evade: walk 175 vs zombie shamble 150 = tense slow pursuit.
		if (!bPatrolGaitOverridden) { PrePatrolDesiredGait = GaitBefore; }
		bPatrolGaitOverridden = true;
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void AEmergeCharacter::StopPatrolEvade()
{
	bPatrolling = false;
	bPatrolLegActive = false;
	PatrolPoints.Reset();
	PatrolIdx = 0;
	StopEvading();   // StartPatrolEvade implied StartEvading, so stop tears both layers down
}

void AEmergeCharacter::TickEvade(const float DeltaSeconds)
{
	UWorld* World = GetWorld();
	if (!World || !EvadeThreat.IsValid())
	{
		StopEvading();   // threat despawned: nothing left to evade
		return;
	}
	const FVector ThreatPos = EvadeThreat->GetActorLocation();
	EvadeThreatDist = FVector::Dist2D(GetActorLocation(), ThreatPos);

	// Slowly drifting bias angle: re-rolled ±25° every 5-10s so ring picks never settle into a
	// predictable orbit around the threat.
	if (World->GetTimeSeconds() >= EvadeBiasRerollTime)
	{
		EvadeBiasAngleDeg = FMath::FRandRange(-25.0f, 25.0f);
		EvadeBiasRerollTime = World->GetTimeSeconds() + FMath::FRandRange(5.0f, 10.0f);
	}

	// Beyond the comfort band: patrol the circuit if patrolling, else STOP and wait — don't jog
	// to the horizon (measured: gap grew 1075->8842uu in 25s; a one-way footrace is not
	// avoidance). Hysteresis at 90% so the boundary doesn't flutter. Evasion re-engages the
	// moment the threat closes back in.
	if (EvadeThreatDist > EvadeComfortRadius)
	{
		// Tether (patrol layer): the zombie has fallen too far behind — STOP and wait for it, even
		// mid-checkpoint-leg (extends the comfort-stop idea), so he never laps away from the chase.
		// Movement resumes naturally below once the threat closes back under the tether.
		if (bPatrolling && EvadeThreatDist > EvadeTetherRadius)
		{
			if (bNavigating) { StopNavigating(); }
			bPatrolLegActive = false;
			EvadeGoal = FVector::ZeroVector;
			EvadeReplanTimer = 0.0f;
			bEvadeWasInDanger = false;
			return;
		}
		if (bPatrolling && PatrolPoints.IsValidIndex(PatrolIdx))
		{
			// Patrol layer: comfortable = lap the checkpoints instead of standing around.
			if (FVector::Dist2D(GetActorLocation(), PatrolPoints[PatrolIdx]) <= PatrolArriveRadius)
			{
				PatrolIdx = (PatrolIdx + 1) % PatrolPoints.Num();   // wrap: endless laps
				EvadeGoal = FVector::ZeroVector;
				bPatrolLegActive = NavigateTo(PatrolPoints[PatrolIdx]);
			}
			// (Re)issue the leg when idle; redirecting an in-flight FLEE goal back to the CURRENT
			// checkpoint waits out the same 10% hysteresis as stop-and-wait (no boundary flutter).
			else if (!bNavigating || (!bPatrolLegActive && EvadeThreatDist > EvadeComfortRadius * 1.1f))
			{
				EvadeGoal = FVector::ZeroVector;   // forget the old flee goal; patrol owns the leg
				bPatrolLegActive = NavigateTo(PatrolPoints[PatrolIdx]);
			}
		}
		else if (bNavigating && EvadeThreatDist > EvadeComfortRadius * 1.1f)
		{
			StopNavigating();
			EvadeGoal = FVector::ZeroVector;   // comfort reached: forget the old flee goal
		}
		EvadeReplanTimer = 0.0f;               // replan instantly on re-entry
		bEvadeWasInDanger = false;
		return;
	}
	bPatrolLegActive = false;   // inside the comfort band: evasion owns navigation again

	// Replan on timer OR immediately when the threat crosses under the danger radius (event,
	// don't wait the timer out).
	const bool bInDanger = EvadeThreatDist < EvadeDangerRadius;
	const bool bDangerCrossed = bInDanger && !bEvadeWasInDanger;
	bEvadeWasInDanger = bInDanger;
	EvadeReplanTimer -= DeltaSeconds;
	if (EvadeReplanTimer <= 0.0f || bDangerCrossed)
	{
		// Failed replan (no candidates at all): retry quickly instead of standing still a full cadence.
		EvadeReplanTimer = ReplanEvade(ThreatPos) ? EvadeReplanSeconds : 0.25f;
	}
}

bool AEmergeCharacter::ReplanEvade(const FVector& ThreatPos)
{
	UWorld* World = GetWorld();
	UNavigationSystemV1* Nav = World ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(World) : nullptr;
	if (!Nav) { return false; }

	const FVector Self = GetActorLocation();
	// Ground-plane away direction with the same degenerate-threat fallback as FleeFrom.
	FVector Away = UEmergeFlee::FleeDirection(Self, ThreatPos);
	Away.Z = 0.0f;
	Away = Away.GetSafeNormal();
	if (Away.IsNearlyZero())
	{
		Away = -GetActorForwardVector();
		Away.Z = 0.0f;
		Away = Away.GetSafeNormal();
		if (Away.IsNearlyZero()) { return false; }
	}
	const FVector2D Away2D(Away.X, Away.Y);
	FVector ThreatDir = ThreatPos - Self;
	ThreatDir.Z = 0.0f;
	const FVector2D ThreatDir2D = FVector2D(ThreatDir.X, ThreatDir.Y).GetSafeNormal();

	// Track-tied evasion (patrol layer): constrain the accepted candidates to ±EvadeTrackConeDeg
	// around the direction to the CURRENT checkpoint, so fleeing keeps progressing along the
	// circuit and drags the zombie across the obstacle stations. The unrestricted best is still
	// tracked as a fallback (fully blocked cone) and the cornered tripwire below stays exempt.
	bool bTrackCone = false;
	FVector2D TrackDir2D = FVector2D::ZeroVector;
	float TrackConeMinDot = -1.0f;
	if (bPatrolling && PatrolPoints.IsValidIndex(PatrolIdx))
	{
		TrackDir2D = FVector2D(PatrolPoints[PatrolIdx].X - Self.X, PatrolPoints[PatrolIdx].Y - Self.Y).GetSafeNormal();
		if (!TrackDir2D.IsNearlyZero())
		{
			bTrackCone = true;
			TrackConeMinDot = FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(EvadeTrackConeDeg, 5.0f, 180.0f)));
		}
	}

	// Candidate rings around SELF: evenly spaced, plus a fresh per-replan jitter and the drifting
	// bias angle. The away-from-threat term below supplies the directionality; sampling the full
	// circle keeps tangential and even threat-side routes on the table (they can be the only way
	// out of a pocket).
	const float JitterDeg = FMath::FRandRange(-18.0f, 18.0f);
	const int32 PerRing = FMath::Max(1, EvadeCandidatesPerRing);
	int32 CandidateCount = 0;
	int32 FailedCount = 0;      // failed projection or no complete path
	float BestScore = -FLT_MAX;
	FVector BestGoal = FVector::ZeroVector;
	bool bFound = false;
	float BestPerp = FLT_MAX;   // |dot(candidate dir, threat dir)| — smaller = more perpendicular
	FVector BestPerpGoal = FVector::ZeroVector;
	bool bPerpFound = false;
	float BestConeScore = -FLT_MAX;   // best candidate INSIDE the track cone (patrol layer)
	FVector BestConeGoal = FVector::ZeroVector;
	bool bConeFound = false;

	for (const float Ring : EvadeRingRadii)
	{
		for (int32 i = 0; i < PerRing; ++i)
		{
			++CandidateCount;
			const float AngleDeg = (360.0f / PerRing) * i + JitterDeg + EvadeBiasAngleDeg;
			const FVector Dir = Away.RotateAngleAxis(AngleDeg, FVector::UpVector);
			FNavLocation Projected;
			if (!Nav->ProjectPointToNavigation(Self + Dir * Ring, Projected, FVector(500.0f, 500.0f, 1000.0f)))
			{
				++FailedCount;
				continue;
			}
			UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, Self, Projected.Location);
			if (!Path || !Path->IsValid() || Path->IsPartial() || Path->PathPoints.Num() < 2)
			{
				++FailedCount;
				continue;
			}

			// THE key upgrade over FleeFrom: score by the whole path's clearance from the threat's
			// CURRENT position — a far endpoint whose route passes right next to the zombie loses.
			float MinPathClear = FLT_MAX;
			for (const FVector& Point : Path->PathPoints)
			{
				MinPathClear = FMath::Min(MinPathClear, (float)FVector::Dist2D(Point, ThreatPos));
			}
			const float Euclid = FMath::Max((float)FVector::Dist2D(Self, Projected.Location), 1.0f);
			const float PocketPenalty = FMath::Max(0.0f, Path->GetPathLength() / Euclid - 1.4f) * 300.0f;
			const FVector2D CandDir = FVector2D(Projected.Location.X - Self.X, Projected.Location.Y - Self.Y).GetSafeNormal();
			const float AwayDot = FVector2D::DotProduct(CandDir, Away2D);
			float Score = 1.0f * MinPathClear - 0.6f * PocketPenalty + 0.25f * AwayDot * 300.0f;

			// Hysteresis: a candidate near the current goal gets a bonus so we only redirect when
			// the new best genuinely beats staying the course by EvadeHysteresisPct.
			if (bNavigating && !EvadeGoal.IsZero() && FVector::Dist2D(Projected.Location, EvadeGoal) < 150.0f && Score > 0.0f)
			{
				Score *= 1.0f + EvadeHysteresisPct;
			}

			if (Score > BestScore) { BestScore = Score; BestGoal = Projected.Location; bFound = true; }
			if (bTrackCone && FVector2D::DotProduct(CandDir, TrackDir2D) >= TrackConeMinDot
				&& Score > BestConeScore)
			{
				BestConeScore = Score;
				BestConeGoal = Projected.Location;
				bConeFound = true;
			}
			const float Perp = FMath::Abs(FVector2D::DotProduct(CandDir, ThreatDir2D));
			if (Perp < BestPerp) { BestPerp = Perp; BestPerpGoal = Projected.Location; bPerpFound = true; }
		}
	}

	// Track cone wins when it produced anything; otherwise the unrestricted best stands in (a
	// stalled NPC pinned to the track line is worse than briefly leaving the circuit).
	if (bTrackCone && bConeFound)
	{
		BestScore = BestConeScore;
		BestGoal = BestConeGoal;
		bFound = true;
	}

	// Cornered tripwire: threat on top of us and most candidates dead -> tangential escape. Take
	// the most PERPENDICULAR surviving candidate regardless of its distance score (running "past"
	// the zombie sideways beats being pinned against the wall behind us).
	bEvadeCornered = FVector::Dist2D(Self, ThreatPos) < 250.0f
		&& CandidateCount > 0 && (float)FailedCount / (float)CandidateCount > 0.6f;
	if (bEvadeCornered && bPerpFound)
	{
		BestGoal = BestPerpGoal;
		bFound = true;
	}
	if (!bFound) { return false; }

	if (!NavigateTo(BestGoal)) { return false; }
	EvadeGoal = BestGoal;
	EvadeGoalScore = BestScore;
	return true;
}
