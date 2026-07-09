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


FString AEmergeCharacter::SenseEnvironment(float Radius)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return TEXT("{\"error\":\"no world\"}");
	}

	const FVector Pos = GetActorLocation();
	const FVector Vel = GetVelocity();
	const FVector Fwd = GetActorForwardVector();
	const FVector Origin = Pos + FVector(0.0f, 0.0f, 40.0f);

	// Game camera POV (from the player camera manager, not the editor camera).
	FString Cam = TEXT("null");
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (const APlayerCameraManager* CM = PC->PlayerCameraManager)
		{
			const FVector CL = CM->GetCameraLocation();
			const FRotator CR = CM->GetCameraRotation();
			Cam = FString::Printf(TEXT("{\"loc\":[%.0f,%.0f,%.0f],\"rot\":[%.1f,%.1f],\"fov\":%.0f}"),
				CL.X, CL.Y, CL.Z, CR.Pitch, CR.Yaw, CM->GetFOVAngle());
		}
	}

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	// LIDAR ring: 16 rays every 22.5 deg (relative to actor forward) + floor + ceiling.
	TArray<FString> Rays;
	const int32 N = 16;
	for (int32 i = 0; i < N; ++i)
	{
		const float Ang = (360.0f / N) * i;
		const FVector Dir = GetActorRotation().RotateVector(FRotator(0.0f, Ang, 0.0f).RotateVector(FVector::ForwardVector));
		FHitResult Hit;
		const bool bHit = World->LineTraceSingleByChannel(Hit, Origin, Origin + Dir * 5000.0f, ECC_Visibility, TraceParams);
		Rays.Add(FString::Printf(TEXT("{\"a\":%.0f,\"d\":%.0f,\"hit\":\"%s\"}"),
			Ang, bHit ? Hit.Distance : 5000.0f, bHit && Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("none")));
	}
	FHitResult FloorHit;
	const bool bFloor = World->LineTraceSingleByChannel(FloorHit, Origin, Origin - FVector(0, 0, 5000.0f), ECC_Visibility, TraceParams);
	FHitResult CeilHit;
	const bool bCeil = World->LineTraceSingleByChannel(CeilHit, Origin, Origin + FVector(0, 0, 5000.0f), ECC_Visibility, TraceParams);

	// Nearby actors (characters + static meshes) within Radius, nearest first, capped.
	struct FNear { float Dist; FString Line; };
	TArray<FNear> Near;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* A = *It;
		if (!A || A == this)
		{
			continue;
		}
		const bool bInteresting = A->IsA(ACharacter::StaticClass()) || A->IsA(APawn::StaticClass()) || A->IsA(AStaticMeshActor::StaticClass());
		if (!bInteresting)
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
	for (int32 i = 0; i < Near.Num() && i < 25; ++i)
	{
		ActorLines.Add(Near[i].Line);
	}

	return FString::Printf(
		TEXT("{\"pos\":[%.0f,%.0f,%.0f],\"vel\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"fwd\":[%.2f,%.2f,%.2f],")
		TEXT("\"cam\":%s,\"floor_d\":%.0f,\"ceil_d\":%.0f,\"rays\":[%s],\"actors\":[%s]}"),
		Pos.X, Pos.Y, Pos.Z, Vel.X, Vel.Y, Vel.Z, Vel.Size2D(), Fwd.X, Fwd.Y, Fwd.Z,
		*Cam, bFloor ? FloorHit.Distance : -1.0f, bCeil ? CeilHit.Distance : -1.0f,
		*FString::Join(Rays, TEXT(",")), *FString::Join(ActorLines, TEXT(",")));
}
