#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeInfection.generated.h"

// Staged wound infection (DayZ model). Untreated dirty wounds infect on a timer: stage 1 (curable ONLY
// by disinfectant) -> stage 2 (fever, needs antibiotics) -> stage 3 (critical). Deterministic, data-free
// pure math so it is headless-testable.
UCLASS()
class EMERGECORE_API UEmergeInfection : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Clean wounds never infect. Dirty wounds progress: <20m -> 0, <40m -> 1, <60m -> 2, else 3.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Infection")
	static int32 StageForMinutes(float MinutesSinceWound, bool bWoundClean);

	// Disinfectant (3) cures stage 1 only; antibiotics (4) cure stage 2 only; nothing cures stage 3.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Infection")
	static bool ItemCuresStage(int32 ItemType, int32 Stage);

	// Stamina regen multiplier by stage: 0->1.0, 1->0.75, 2->0.5, 3+->0.25.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Infection")
	static float StaminaRegenMultiplierForStage(int32 Stage);
};
