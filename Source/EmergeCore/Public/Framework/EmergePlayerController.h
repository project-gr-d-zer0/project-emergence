#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EmergePlayerController.generated.h"

// Routes all input (incl. mouse buttons) to the pawn. Also exposes DebugConsole for external
// autonomous control via Remote Control (Claude drives the live PIE session from the terminal).
UCLASS()
class EMERGECORE_API AEmergePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Run any console command in the live session (input injection, cvars, stats). BlueprintCallable
	// so the Remote Control HTTP API can invoke it: PUT /remote/object/call DebugConsole { Command }.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Debug")
	FString DebugConsole(const FString& Command);

protected:
	virtual void BeginPlay() override;
};
