#include "Sensor/EmergeWorldSense.h"
#include "Sensor/EmergeStuckDetect.h"
#include "AI/EmergeEnemyController.h"
#include "Nav/EmergeInfluenceGrid.h"
#include "Navigation/PathFollowingComponent.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/WorldSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

// Pose/skeleton oracle for any character: anim instance presence, orientation (upside-down),
// key bone heights, and the T-pose heuristic mirrored from the player's SenseEnvironment bones block.
static FString PoseJsonFor(const APawn* P)
{
	const ACharacter* Char = Cast<ACharacter>(P);
	USkeletalMeshComponent* M = Char ? Char->GetMesh() : nullptr;
	if (!M || !M->GetSkeletalMeshAsset()) { return TEXT("null"); }
	// Wrong-skeleton guard: GetSocketLocation on a missing bone returns the component location (NOT
	// near-zero), so value heuristics can't catch a bad mesh swap — probe the skeleton directly.
	if (!M->DoesSocketExist(TEXT("head")) || !M->DoesSocketExist(TEXT("pelvis"))
		|| !M->DoesSocketExist(TEXT("hand_l")) || !M->DoesSocketExist(TEXT("hand_r"))
		|| !M->DoesSocketExist(TEXT("foot_l")) || !M->DoesSocketExist(TEXT("foot_r")))
	{
		return TEXT("{\"error\":\"missing bones (wrong skeleton?)\"}");
	}
	const float UpZ = M->GetUpVector().Z;
	const FVector Head = M->GetSocketLocation(TEXT("head"));
	const FVector Pelvis = M->GetSocketLocation(TEXT("pelvis"));
	const FVector FootL = M->GetSocketLocation(TEXT("foot_l"));
	const FVector FootR = M->GetSocketLocation(TEXT("foot_r"));
	const FVector HandL = M->GetSocketLocation(TEXT("hand_l"));
	const FVector HandR = M->GetSocketLocation(TEXT("hand_r"));
	const bool bUpsideDown = (UpZ < 0.0f) || (Head.Z < Pelvis.Z - 20.0f);
	// Flip/tumble oracle: mesh tilted well off vertical OR head sinking toward pelvis level —
	// catches mid-traversal tumbles that full inversion checks miss (user-reported flip).
	const bool bTumbling = (UpZ < 0.7f) || (Head.Z < Pelvis.Z + 15.0f);
	const float HandSpread = FVector::Dist(HandL, HandR);
	const bool bTPose = HandSpread > 120.0f
		&& FMath::Abs(HandL.Z - HandR.Z) < 15.0f && FMath::Abs(HandL.Z - Head.Z) < 45.0f;
	const UAnimInstance* Anim = M->GetAnimInstance();
	return FString::Printf(
		TEXT("{\"anim\":%s,\"animPlaying\":%s,\"upZ\":%.2f,\"upsideDown\":%s,\"tumbling\":%s,\"headZ\":%.0f,\"pelvisZ\":%.0f,\"feetZ\":[%.0f,%.0f],\"handSpread\":%.0f,\"tposeSuspect\":%s}"),
		Anim ? *FString::Printf(TEXT("\"%s\""), *Anim->GetClass()->GetName()) : TEXT("null"),
		M->IsPlaying() ? TEXT("true") : TEXT("false"),
		UpZ, bUpsideDown ? TEXT("true") : TEXT("false"), bTumbling ? TEXT("true") : TEXT("false"),
		Head.Z, Pelvis.Z, FootL.Z, FootR.Z,
		HandSpread, bTPose ? TEXT("true") : TEXT("false"));
}

void UEmergeWorldSense::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bReady = true;
}

TStatId UEmergeWorldSense::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UEmergeWorldSense, STATGROUP_Tickables);
}

void UEmergeWorldSense::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World) { return; }
	for (TActorIterator<AEmergeEnemyController> It(World); It; ++It)
	{
		const APawn* P = It->GetPawn();
		if (!P) { continue; }
		const bool bExpectedMoving = It->GetMoveStatus() == EPathFollowingStatus::Moving;
		float& S = StuckSeconds.FindOrAdd(FObjectKey(*It));
		S = UEmergeStuckDetect::Accumulate(S, P->GetVelocity().Size(), StuckSpeedFloor, bExpectedMoving, DeltaTime);
		UpdateBoneOracles(FObjectKey(*It), P, DeltaTime);
	}
	// Player gets the same bone oracles (foot-slide catches "unnatural run" on either character).
	if (const APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		UpdateBoneOracles(FObjectKey(Player), Player, DeltaTime);
	}
}

void UEmergeWorldSense::UpdateBoneOracles(const FObjectKey& Key, const APawn* P, float DeltaTime)
{
	const ACharacter* Char = Cast<ACharacter>(P);
	USkeletalMeshComponent* M = Char ? Char->GetMesh() : nullptr;
	if (!M || !M->GetSkeletalMeshAsset() || DeltaTime <= 0.0f) { return; }

	// Anim liveness: hand motion relative to the actor.
	const FVector Rel = M->GetSocketLocation(TEXT("hand_r")) - P->GetActorLocation();
	if (const FVector* Prev = PrevHandRel.Find(Key))
	{
		const float Rate = (Rel - *Prev).Size() / DeltaTime;
		float& Smoothed = BoneMotionUu.FindOrAdd(Key);
		Smoothed = FMath::Lerp(Smoothed, Rate, 0.2f);
	}
	PrevHandRel.Add(Key, Rel);

	// Foot slide: horizontal drift of the LOWER (planted) foot while grounded.
	if (Char->GetCharacterMovement() && Char->GetCharacterMovement()->IsMovingOnGround()
		&& P->GetVelocity().Size2D() > 30.0f)
	{
		const FVector FootL = M->GetSocketLocation(TEXT("foot_l"));
		const FVector FootR = M->GetSocketLocation(TEXT("foot_r"));
		const FVector Planted = (FootL.Z <= FootR.Z) ? FootL : FootR;
		if (const FVector* PrevFoot = PrevPlantedFoot.Find(Key))
		{
			// Only meaningful while the same foot stays planted; the min() with the raw rate keeps
			// swap frames from spiking the average.
			const float SlideRate = FVector::Dist2D(Planted, *PrevFoot) / DeltaTime;
			float& Smoothed = FootSlideUu.FindOrAdd(Key);
			Smoothed = FMath::Lerp(Smoothed, FMath::Min(SlideRate, 600.0f), 0.15f);
		}
		PrevPlantedFoot.Add(Key, Planted);
	}
}

FString UEmergeWorldSense::SenseSkeleton(const FString& NameFilter)
{
	UWorld* World = GetWorld();
	if (!World) { return TEXT("{}"); }
	const ACharacter* Target = nullptr;
	if (NameFilter.IsEmpty())
	{
		Target = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(World, 0));
	}
	else
	{
		for (TActorIterator<ACharacter> It(World); It; ++It)
		{
			if (It->GetName().Contains(NameFilter)) { Target = *It; break; }
		}
	}
	USkeletalMeshComponent* M = Target ? Target->GetMesh() : nullptr;
	if (!M || !M->GetSkeletalMeshAsset()) { return TEXT("{\"error\":\"no match\"}"); }

	const FVector Origin = Target->GetActorLocation();
	FString Bones;
	const int32 Num = M->GetNumBones();
	for (int32 i = 0; i < Num; ++i)
	{
		const FName BoneName = M->GetBoneName(i);
		const FVector BP = M->GetBoneLocation(BoneName) - Origin;   // actor-relative: compact + comparable
		const FRotator BR = M->GetBoneQuaternion(BoneName).Rotator();
		Bones += FString::Printf(TEXT("%s{\"n\":\"%s\",\"p\":[%.0f,%.0f,%.0f],\"r\":[%.0f,%.0f,%.0f]}"),
			Bones.IsEmpty() ? TEXT("") : TEXT(","), *BoneName.ToString(),
			BP.X, BP.Y, BP.Z, BR.Pitch, BR.Yaw, BR.Roll);
	}
	return FString::Printf(TEXT("{\"actor\":\"%s\",\"origin\":[%.0f,%.0f,%.0f],\"boneCount\":%d,\"bones\":[%s]}"),
		*Target->GetName(), Origin.X, Origin.Y, Origin.Z, Num, *Bones);
}

FString UEmergeWorldSense::SenseWorld()
{
	UWorld* World = GetWorld();
	if (!World) { return TEXT("{}"); }
	const AWorldSettings* Settings = World->GetWorldSettings();
	const float KillZ = Settings ? Settings->KillZ : -HALF_WORLD_MAX;

	FString PlayerJson = TEXT("null");
	if (const APawn* Player = UGameplayStatics::GetPlayerPawn(World, 0))
	{
		const FVector P = Player->GetActorLocation();
		PlayerJson = FString::Printf(TEXT("{\"pos\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"footSlideUu\":%.0f,\"pose\":%s}"),
			P.X, P.Y, P.Z, Player->GetVelocity().Size(),
			FootSlideUu.FindRef(FObjectKey(Player)), *PoseJsonFor(Player));
	}

	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	const bool bNavReady = Nav && Nav->MainNavData != nullptr;
	const bool bNavBuilding = Nav && Nav->IsNavigationBuildInProgress();

	static const TCHAR* StateNames[] = { TEXT("Unaware"), TEXT("Suspicious"), TEXT("Investigating"), TEXT("Chasing"), TEXT("Searching") };
	static const TCHAR* MoveNames[] = { TEXT("Idle"), TEXT("Waiting"), TEXT("Paused"), TEXT("Moving") };
	int32 Total = 0;
	int32 StateCounts[5] = { 0, 0, 0, 0, 0 };
	FString Enemies, Anomalies;
	for (TActorIterator<AEmergeEnemyController> It(World); It; ++It)
	{
		const APawn* P = It->GetPawn();
		if (!P) { continue; }
		const FVector L = P->GetActorLocation();
		const int32 StateIdx = FMath::Clamp((int32)It->AIState, 0, 4);
		const int32 MoveIdx = FMath::Clamp((int32)It->GetMoveStatus(), 0, 3);
		const float Stuck = StuckSeconds.FindRef(FObjectKey(*It));
		const bool bBelowKillZ = L.Z < KillZ;
		const bool bStuck = UEmergeStuckDetect::IsStuck(Stuck, StuckAlarmSeconds);
		const FString Pose = PoseJsonFor(P);
		const bool bUpsideDown = Pose.Contains(TEXT("\"upsideDown\":true"));
		const bool bTPose = Pose.Contains(TEXT("\"tposeSuspect\":true"));
		const bool bNoAnim = Pose.Contains(TEXT("\"anim\":null"));
		++Total;
		++StateCounts[StateIdx];
		Enemies += FString::Printf(
			TEXT("%s{\"name\":\"%s\",\"state\":\"%s\",\"awareness\":%.2f,\"pos\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"move\":\"%s\",\"targetVisible\":%s,\"dist\":%.0f,\"stuckSec\":%.1f,\"belowKillZ\":%s,\"boneMotionUu\":%.0f,\"footSlideUu\":%.0f,\"pose\":%s}"),
			Enemies.IsEmpty() ? TEXT("") : TEXT(","), *It->GetName(), StateNames[StateIdx], It->Awareness,
			L.X, L.Y, L.Z, P->GetVelocity().Size(), MoveNames[MoveIdx],
			It->bTargetVisible ? TEXT("true") : TEXT("false"), It->GetDistanceToTarget(), Stuck,
			bBelowKillZ ? TEXT("true") : TEXT("false"), BoneMotionUu.FindRef(FObjectKey(*It)),
			FootSlideUu.FindRef(FObjectKey(*It)), *Pose);
		if (bStuck || bBelowKillZ || bUpsideDown || bTPose || bNoAnim)
		{
			Anomalies += FString::Printf(
				TEXT("%s{\"name\":\"%s\",\"stuck\":%s,\"belowKillZ\":%s,\"upsideDown\":%s,\"tpose\":%s,\"noAnim\":%s}"),
				Anomalies.IsEmpty() ? TEXT("") : TEXT(","), *It->GetName(),
				bStuck ? TEXT("true") : TEXT("false"), bBelowKillZ ? TEXT("true") : TEXT("false"),
				bUpsideDown ? TEXT("true") : TEXT("false"), bTPose ? TEXT("true") : TEXT("false"),
				bNoAnim ? TEXT("true") : TEXT("false"));
		}
	}

	FString Grid = TEXT("null");
	if (UEmergeInfluenceGrid* Influence = World->GetSubsystem<UEmergeInfluenceGrid>())
	{
		Grid = Influence->GetGridStatus();
	}

	return FString::Printf(
		TEXT("{\"time\":%.1f,\"frameMs\":%.1f,\"player\":%s,\"nav\":{\"ready\":%s,\"building\":%s},")
		TEXT("\"enemies\":[%s],\"counts\":{\"total\":%d,\"unaware\":%d,\"suspicious\":%d,\"investigating\":%d,\"chasing\":%d,\"searching\":%d},")
		TEXT("\"influence\":%s,\"anomalies\":[%s]}"),
		World->GetTimeSeconds(), World->GetDeltaSeconds() * 1000.0f, *PlayerJson,
		bNavReady ? TEXT("true") : TEXT("false"), bNavBuilding ? TEXT("true") : TEXT("false"),
		*Enemies, Total, StateCounts[0], StateCounts[1], StateCounts[2], StateCounts[3], StateCounts[4],
		*Grid, *Anomalies);
}
