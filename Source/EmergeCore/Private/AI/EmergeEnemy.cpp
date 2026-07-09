#include "AI/EmergeEnemy.h"
#include "AI/EmergeEnemyController.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}
