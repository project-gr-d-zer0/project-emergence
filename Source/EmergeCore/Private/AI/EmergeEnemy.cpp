#include "AI/EmergeEnemy.h"
#include "AI/EmergeEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"

AEmergeEnemy::AEmergeEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AEmergeEnemyController::StaticClass();
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = 150.0f;      // slow shamble; the controller bumps this when chasing
		Move->bUseRVOAvoidance = false;   // Detour crowd (via the controller) handles avoidance instead
	}

	// Visible, animated body (UE5 Mannequin + unarmed locomotion anim BP).
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
			TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
		if (MeshAsset.Succeeded()) { MeshComp->SetSkeletalMesh(MeshAsset.Object); }
		static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(
			TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
		if (AnimBP.Succeeded()) { MeshComp->SetAnimInstanceClass(AnimBP.Class); }
		MeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	}
}
