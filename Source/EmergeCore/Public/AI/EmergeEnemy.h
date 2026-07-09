#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EmergeEnemy.generated.h"

// Phase A enemy: a plain ACharacter (has CharacterMovement so AIController MoveTo works). Slow by default
// (shamble); the controller raises speed when chasing. Auto-possessed by AEmergeEnemyController.
UCLASS()
class EMERGECORE_API AEmergeEnemy : public ACharacter
{
	GENERATED_BODY()
public:
	AEmergeEnemy();
};
