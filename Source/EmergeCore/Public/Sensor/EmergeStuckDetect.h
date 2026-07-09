#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStuckDetect.generated.h"

// NPC stuck-detection oracle math. Mirrors the player's pressedNotMoving glitch oracle for AI
// pawns: accumulates time an agent is expected to be moving but isn't; resets the moment it
// moves or stops trying.
UCLASS()
class EMERGECORE_API UEmergeStuckDetect : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static float Accumulate(float Current, float Speed, float MinSpeed, bool bExpectedMoving, float DtSeconds);

	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static bool IsStuck(float StuckSeconds, float ThresholdSeconds);
};
