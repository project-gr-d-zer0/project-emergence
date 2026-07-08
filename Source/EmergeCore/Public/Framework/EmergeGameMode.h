#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EmergeGameMode.generated.h"

// Minimal game mode for the first playable slice. Spawn rules / world state are added later;
// this exists so the editor map can assign a concrete Emerge GameMode and Blueprints can derive from it.
UCLASS()
class EMERGECORE_API AEmergeGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AEmergeGameMode();
};
