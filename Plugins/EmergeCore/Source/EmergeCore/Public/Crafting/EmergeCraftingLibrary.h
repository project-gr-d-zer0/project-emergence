#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCraftingLibrary.generated.h"

// Pure, deterministic crafting-time formula (no RNG, no wall-clock):
//   CompletionTime = BaseTime * TierModifier / max(1, WorkerCount)
UCLASS()
class EMERGECORE_API UEmergeCraftingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Emerge|Crafting")
	static float CalculateCompletionTime(float BaseTime, float TierModifier, int32 WorkerCount);
};
