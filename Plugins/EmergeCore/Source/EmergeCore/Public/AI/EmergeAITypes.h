#pragma once
#include "CoreMinimal.h"
#include "EmergeAITypes.generated.h"

// Detection/behaviour stages for the Phase A enemy FSM. Codes 0..3 align with UEmergeAwareness::StateFor;
// Searching is the separate "lost the target, hunting last-known-position" stage.
UENUM(BlueprintType)
enum class EEmergeAIState : uint8
{
	Unaware       UMETA(DisplayName = "Unaware"),
	Suspicious    UMETA(DisplayName = "Suspicious"),
	Investigating UMETA(DisplayName = "Investigating"),
	Chasing       UMETA(DisplayName = "Chasing"),
	Searching     UMETA(DisplayName = "Searching")
};
