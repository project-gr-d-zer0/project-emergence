#include "AI/EmergeEnemy.h"
#include "AI/EmergeEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "UObject/ConstructorHelpers.h"

AEmergeEnemy::AEmergeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEmergeEnemyController::StaticClass();
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = 150.0f;      // slow shamble; the controller bumps this when chasing
		Move->bUseRVOAvoidance = false;   // Detour crowd (via the controller) handles avoidance instead
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
			TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
		if (MeshAsset.Succeeded()) { MeshComp->SetSkeletalMesh(MeshAsset.Object); }
		static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> Idle(
			TEXT("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle.MM_Idle"));
		if (Idle.Succeeded()) { IdleLoop = Idle.Object; }
		static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> Walk(
			TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd.MF_Unarmed_Walk_Fwd"));
		if (Walk.Succeeded()) { WalkLoop = Walk.Object; }
		static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> Jog(
			TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Jog/MF_Unarmed_Jog_Fwd.MF_Unarmed_Jog_Fwd"));
		if (Jog.Succeeded()) { JogLoop = Jog.Object; }
		MeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
		// Always evaluate the pose: render-gated anim ticking freezes offscreen/unfocused PIE poses.
		MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
}

void AEmergeEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) { return; }
	const float Spd = GetVelocity().Size2D();
	UAnimSequenceBase* Want = (Spd > 250.0f) ? JogLoop.Get() : ((Spd > 5.0f) ? WalkLoop.Get() : IdleLoop.Get());
	if (Want && CurrentLoop != Want)
	{
		MeshComp->PlayAnimation(Want, true);
		CurrentLoop = Want;
	}
}

FString AEmergeEnemy::GetAnimDebug() const
{
	const USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) { return TEXT("{\"error\":\"no mesh\"}"); }
	const UAnimSingleNodeInstance* SingleNode = MeshComp->GetSingleNodeInstance();
	const UAnimationAsset* Asset = SingleNode ? SingleNode->GetAnimationAsset() : nullptr;
	return FString::Printf(
		TEXT("{\"mode\":%d,\"playing\":%s,\"asset\":%s,\"idle\":%s,\"walk\":%s,\"jog\":%s,\"spd\":%.0f}"),
		(int32)MeshComp->GetAnimationMode(),
		MeshComp->IsPlaying() ? TEXT("true") : TEXT("false"),
		Asset ? *FString::Printf(TEXT("\"%s\""), *Asset->GetName()) : TEXT("null"),
		IdleLoop ? TEXT("true") : TEXT("false"), WalkLoop ? TEXT("true") : TEXT("false"),
		JogLoop ? TEXT("true") : TEXT("false"), GetVelocity().Size2D());
}
