#include "Framework/EmergeGameMode.h"
#include "Framework/EmergePlayerController.h"

AEmergeGameMode::AEmergeGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	PlayerControllerClass = AEmergePlayerController::StaticClass();
}
