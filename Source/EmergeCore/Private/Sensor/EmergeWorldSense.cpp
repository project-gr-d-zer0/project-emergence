#include "Sensor/EmergeWorldSense.h"
#include "Sensor/EmergeStuckDetect.h"
#include "AI/EmergeEnemyController.h"
#include "Nav/EmergeInfluenceGrid.h"
#include "Navigation/PathFollowingComponent.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
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
	const float UpZ = M->GetUpVector().Z;
	const FVector Head = M->GetSocketLocation(TEXT("head"));
	const FVector Pelvis = M->GetSocketLocation(TEXT("pelvis"));
	const FVector FootL = M->GetSocketLocation(TEXT("foot_l"));
	const FVector FootR = M->GetSocketLocation(TEXT("foot_r"));
	const FVector HandL = M->GetSocketLocation(TEXT("hand_l"));
	const FVector HandR = M->GetSocketLocation(TEXT("hand_r"));
	const bool bUpsideDown = (UpZ < 0.0f) || (Head.Z < Pelvis.Z - 20.0f);
	const float HandSpread = FVector::Dist(HandL, HandR);
	const bool bTPose = HandSpread > 120.0f
		&& FMath::Abs(HandL.Z - HandR.Z) < 15.0f && FMath::Abs(HandL.Z - Head.Z) < 45.0f;
	const UAnimInstance* Anim = M->GetAnimInstance();
	return FString::Printf(
		TEXT("{\"anim\":%s,\"animPlaying\":%s,\"upZ\":%.2f,\"upsideDown\":%s,\"headZ\":%.0f,\"pelvisZ\":%.0f,\"feetZ\":[%.0f,%.0f],\"handSpread\":%.0f,\"tposeSuspect\":%s}"),
		Anim ? *FString::Printf(TEXT("\"%s\""), *Anim->GetClass()->GetName()) : TEXT("null"),
		M->IsPlaying() ? TEXT("true") : TEXT("false"),
		UpZ, bUpsideDown ? TEXT("true") : TEXT("false"), Head.Z, Pelvis.Z, FootL.Z, FootR.Z,
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

		if (const ACharacter* Char = Cast<ACharacter>(P))
		{
			if (USkeletalMeshComponent* M = Char->GetMesh())
			{
				const FVector Rel = M->GetSocketLocation(TEXT("hand_r")) - P->GetActorLocation();
				if (const FVector* Prev = PrevHandRel.Find(FObjectKey(*It)))
				{
					const float Rate = DeltaTime > 0.0f ? (Rel - *Prev).Size() / DeltaTime : 0.0f;
					float& Smoothed = BoneMotionUu.FindOrAdd(FObjectKey(*It));
					Smoothed = FMath::Lerp(Smoothed, Rate, 0.2f);
				}
				PrevHandRel.Add(FObjectKey(*It), Rel);
			}
		}
	}
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
		PlayerJson = FString::Printf(TEXT("{\"pos\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"pose\":%s}"),
			P.X, P.Y, P.Z, Player->GetVelocity().Size(), *PoseJsonFor(Player));
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
			TEXT("%s{\"name\":\"%s\",\"state\":\"%s\",\"awareness\":%.2f,\"pos\":[%.0f,%.0f,%.0f],\"spd\":%.0f,\"move\":\"%s\",\"targetVisible\":%s,\"dist\":%.0f,\"stuckSec\":%.1f,\"belowKillZ\":%s,\"boneMotionUu\":%.0f,\"pose\":%s}"),
			Enemies.IsEmpty() ? TEXT("") : TEXT(","), *It->GetName(), StateNames[StateIdx], It->Awareness,
			L.X, L.Y, L.Z, P->GetVelocity().Size(), MoveNames[MoveIdx],
			It->bTargetVisible ? TEXT("true") : TEXT("false"), It->GetDistanceToTarget(), Stuck,
			bBelowKillZ ? TEXT("true") : TEXT("false"), BoneMotionUu.FindRef(FObjectKey(*It)), *Pose);
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
