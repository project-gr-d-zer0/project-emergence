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

	// ALS gameplay + movement settings (required or the animation refresh early-returns).
	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> AlsSettings(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (AlsSettings.Succeeded()) { Settings = AlsSettings.Object; }
	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> AlsMovement(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (AlsMovement.Succeeded()) { MovementSettings = AlsMovement.Object; }

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

	FVector CamLoc = Origin;
	FVector CamFwd = Fwd;
	float FOV = 90.0f;
	if (FollowCamera)
	{
		CamLoc = FollowCamera->GetComponentLocation();
		CamFwd = FollowCamera->GetForwardVector();
		FOV = FollowCamera->FieldOfView;
	}

	// Aim trace: what the camera is pointing at.
	FString Aim = TEXT("null");
	{
		FHitResult AimHit;
		if (World->LineTraceSingleByChannel(AimHit, CamLoc, CamLoc + CamFwd * 15000.0f, ECC_Visibility, TP) && AimHit.GetActor())
		{
			Aim = FString::Printf(TEXT("{\"hit\":\"%s\",\"cls\":\"%s\",\"d\":%.0f,\"pt\":[%.0f,%.0f,%.0f]}"),
				*AimHit.GetActor()->GetName(), *AimHit.GetActor()->GetClass()->GetName(), AimHit.Distance,
				AimHit.ImpactPoint.X, AimHit.ImpactPoint.Y, AimHit.ImpactPoint.Z);
		}
	}

	// LIDAR ring: categorized, normalized distance, surface normal Z (floor vs wall).
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
		float NormD = 1.0f;
		float Nz = 0.0f;
		FString HitName = TEXT("none");
		if (bHit)
		{
			NormD = Hit.Distance / RayLen;
			Nz = Hit.ImpactNormal.Z;
			AActor* HA = Hit.GetActor();
			HitName = HA ? HA->GetName() : TEXT("?");
			if (HA && HA->IsA(APawn::StaticClass())) { Cat = TEXT("character"); }
			else if (HitName.Contains(TEXT("build")) || HitName.Contains(TEXT("wall")) || HitName.Contains(TEXT("pillar")) || HitName.Contains(TEXT("storefront"))) { Cat = TEXT("building"); }
			else if (HitName.Contains(TEXT("ground")) || HitName.Contains(TEXT("road")) || HitName.Contains(TEXT("walkway")) || HitName.Contains(TEXT("curb"))) { Cat = TEXT("ground"); }
			else { Cat = TEXT("prop"); }
		}
		Rays.Add(FString::Printf(TEXT("{\"a\":%.0f,\"nd\":%.2f,\"cat\":\"%s\",\"nz\":%.2f,\"hit\":\"%s\"}"),
			Ang, NormD, *Cat, Nz, *HitName));
	}

	// Ledge / wall-ahead probes (vault debugging): 3 forward angles.
	TArray<FString> Ledges;
	const float AheadDist = 150.0f;
	for (int32 k = -1; k <= 1; ++k)
	{
		const float Ang = 20.0f * k;
		const FVector Dir = ActorRot.RotateVector(FRotator(0.0f, Ang, 0.0f).RotateVector(FVector::ForwardVector));
		const FVector ProbeTop = Pos + Dir * AheadDist + FVector(0.0f, 0.0f, 60.0f);
		FHitResult FloorAhead;
		const bool bFloorAhead = World->LineTraceSingleByChannel(FloorAhead, ProbeTop, ProbeTop - FVector(0.0f, 0.0f, 400.0f), ECC_Visibility, TP);
		FHitResult WallAhead;
		const FVector Eye = Pos + FVector(0.0f, 0.0f, 40.0f);
		const bool bWall = World->LineTraceSingleByChannel(WallAhead, Eye, Eye + Dir * AheadDist, ECC_Visibility, TP);
		Ledges.Add(FString::Printf(TEXT("{\"a\":%.0f,\"floor\":%s,\"floor_drop\":%.0f,\"ledge\":%s,\"wall\":%s,\"wall_d\":%.0f}"),
			Ang,
			bFloorAhead ? TEXT("true") : TEXT("false"),
			bFloorAhead ? (ProbeTop.Z - FloorAhead.ImpactPoint.Z) : -1.0f,
			(!bFloorAhead) ? TEXT("true") : TEXT("false"),
			bWall ? TEXT("true") : TEXT("false"),
			bWall ? WallAhead.Distance : -1.0f));
	}

	FHitResult FloorHit;
	const bool bFloor = World->LineTraceSingleByChannel(FloorHit, Origin, Origin - FVector(0, 0, 5000.0f), ECC_Visibility, TP);
	FHitResult CeilHit;
	const bool bCeil = World->LineTraceSingleByChannel(CeilHit, Origin, Origin + FVector(0, 0, 5000.0f), ECC_Visibility, TP);

	const float KillZ = World->GetWorldSettings() ? World->GetWorldSettings()->KillZ : -1.0e9f;
	const bool bBelowKill = Pos.Z < KillZ;
	const bool bFellThrough = bFalling && Vel.Z < -50.0f && !bFloor;
	const bool bStuck = bHasInput && (Vel.Size2D() < 10.0f) && bGrounded;

	struct FNear { float Dist; FString Line; };
	TArray<FNear> Near;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* A = *It;
		if (!A || A == this)
		{
			continue;
		}
		if (!(A->IsA(ACharacter::StaticClass()) || A->IsA(APawn::StaticClass()) || A->IsA(AStaticMeshActor::StaticClass())))
		{
			continue;
		}
		const FVector Rel = A->GetActorLocation() - Pos;
		const float Dist = Rel.Size();
		if (Dist > Radius)
		{
			continue;
		}
		Near.Add({ Dist, FString::Printf(TEXT("{\"name\":\"%s\",\"cls\":\"%s\",\"rel\":[%.0f,%.0f,%.0f],\"d\":%.0f}"),
			*A->GetName(), *A->GetClass()->GetName(), Rel.X, Rel.Y, Rel.Z, Dist) });
	}
	Near.Sort([](const FNear& L, const FNear& R) { return L.Dist < R.Dist; });
	TArray<FString> ActorLines;
	for (int32 i = 0; i < Near.Num() && i < 20; ++i)
	{
		ActorLines.Add(Near[i].Line);
	}

	// Limb/joint telemetry: key bone positions relative to the actor root, for verifying animation/pose.
	FString Bones = TEXT("null");
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		auto Rel = [&](const TCHAR* BoneName) -> FString
		{
			const FVector W = SkelMesh->GetSocketLocation(FName(BoneName));
			const FVector R = W - Pos;
			return FString::Printf(TEXT("[%.0f,%.0f,%.0f]"), R.X, R.Y, R.Z);
		};
		const FVector HandL = SkelMesh->GetSocketLocation(FName(TEXT("hand_l")));
		const FVector HandR = SkelMesh->GetSocketLocation(FName(TEXT("hand_r")));
		const FVector Head = SkelMesh->GetSocketLocation(FName(TEXT("head")));
		const float HandSpread = FVector::Dist2D(HandL, HandR);
		const bool bTpose = HandSpread > 120.0f;                 // arms wide + level ~= T-pose (heuristic)
		const bool bValid = !Head.IsNearlyZero();                // (0,0,0) => bones not ticked this frame
		Bones = FString::Printf(
			TEXT("{\"head\":%s,\"hand_l\":%s,\"hand_r\":%s,\"foot_l\":%s,\"foot_r\":%s,\"pelvis\":%s,\"handSpread\":%.0f,\"tposeSuspect\":%s,\"valid\":%s}"),
			*Rel(TEXT("head")), *Rel(TEXT("hand_l")), *Rel(TEXT("hand_r")),
			*Rel(TEXT("foot_l")), *Rel(TEXT("foot_r")), *Rel(TEXT("pelvis")),
			HandSpread, bTpose ? TEXT("true") : TEXT("false"), bValid ? TEXT("true") : TEXT("false"));
	}

	// Animation events/state: ALS locomotion action (Mantling=vault, Ragdolling, Rolling, GettingUp)
	// + gait + active montage, so Claude KNOWS when the vault or any montage animation is triggered.
	FString Anim;
	{
		const FString Action = GetLocomotionAction().IsValid() ? GetLocomotionAction().GetTagName().ToString() : FString(TEXT("none"));
		FString Montage = TEXT("none");
		float MPos = -1.0f;
		if (USkeletalMeshComponent* Mesh2 = GetMesh())
		{
			if (UAnimInstance* AI = Mesh2->GetAnimInstance())
			{
				if (UAnimMontage* M = AI->GetCurrentActiveMontage())
				{
					Montage = M->GetName();
					MPos = AI->Montage_GetPosition(M);
				}
			}
		}
		Anim = FString::Printf(TEXT("{\"action\":\"%s\",\"gait\":\"%s\",\"montage\":\"%s\",\"montagePos\":%.2f}"),
			*Action, *GetGait().GetTagName().ToString(), *Montage, MPos);
	}

	return FString::Printf(
		TEXT("{\"self\":{\"pos\":[%.0f,%.0f,%.0f],\"vel\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"velZ\":%.0f,\"move\":\"%s\",\"grounded\":%s,\"falling\":%s,\"hasInput\":%s},")
		TEXT("\"cam\":{\"loc\":[%.0f,%.0f,%.0f],\"fwd\":[%.2f,%.2f,%.2f],\"fov\":%.0f},")
		TEXT("\"aim\":%s,\"floor_d\":%.0f,\"ceil_d\":%.0f,")
		TEXT("\"rays\":[%s],\"ledges\":[%s],")
		TEXT("\"anomalies\":{\"fellThroughFloor\":%s,\"belowKillZ\":%s,\"stuck\":%s},")
		TEXT("\"actors\":[%s],\"bones\":%s,\"anim\":%s}"),
		Pos.X, Pos.Y, Pos.Z, Vel.X, Vel.Y, Vel.Z, Vel.Size2D(), Vel.Z, *MoveMode,
		bGrounded ? TEXT("true") : TEXT("false"), bFalling ? TEXT("true") : TEXT("false"), bHasInput ? TEXT("true") : TEXT("false"),
		CamLoc.X, CamLoc.Y, CamLoc.Z, CamFwd.X, CamFwd.Y, CamFwd.Z, FOV,
		*Aim, bFloor ? FloorHit.Distance : -1.0f, bCeil ? CeilHit.Distance : -1.0f,
		*FString::Join(Rays, TEXT(",")), *FString::Join(Ledges, TEXT(",")),
		bFellThrough ? TEXT("true") : TEXT("false"), bBelowKill ? TEXT("true") : TEXT("false"), bStuck ? TEXT("true") : TEXT("false"),
		*FString::Join(ActorLines, TEXT(",")), *Bones, *Anim);
}
