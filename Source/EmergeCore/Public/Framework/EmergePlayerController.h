#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EmergePlayerController.generated.h"

// Routes all input (incl. mouse buttons) to the pawn. Without FInputModeGameOnly, PIE leaves the
// cursor active and mouse-button actions (aim/fire) are swallowed by Slate instead of reaching input.
UCLASS()
class EMERGECORE_API AEmergePlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
};
