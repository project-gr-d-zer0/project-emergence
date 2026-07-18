#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeCraftingRecipe.generated.h"

// Data-driven crafting recipe. The completion-time formula itself lives in
// UEmergeCraftingLibrary (pure math -> BlueprintFunctionLibrary); this method forwards to it
// so recipe instances stay a thin, testable data holder.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeCraftingRecipe : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	float BaseTimeSeconds = 0.0f;

	// CompletionTime = BaseTime * TierModifier / max(1, WorkerCount).
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	float CalculateCompletionTime(float BaseTime, float TierModifier, int32 WorkerCount) const;
};
