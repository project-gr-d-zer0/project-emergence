#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNavEta.generated.h"

// ETA + progress telemetry helpers for nav-following (see AEmergeCharacter::GetNavProgress).
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeNavEta : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Estimated time to arrival given remaining distance and current speed.
	// Guards near-zero speed (<= 1.0 uu/s) by returning -1.0 (unknown).
	UFUNCTION(BlueprintPure, Category = "Nav")
	static float EtaSeconds(float RemainingLength, float SpeedUUPerSec);

	// True only when distance remaining closed by at least MinDelta since the previous sample.
	UFUNCTION(BlueprintPure, Category = "Nav")
	static bool MakingProgress(float PrevDistRemaining, float CurDistRemaining, float MinDelta);
};
