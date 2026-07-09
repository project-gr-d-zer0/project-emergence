#include "Framework/EmergePlayerController.h"

void AEmergePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

FString AEmergePlayerController::DebugConsole(const FString& Command)
{
	return ConsoleCommand(Command, true);
}
