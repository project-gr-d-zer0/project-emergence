#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeObjectiveTracker.generated.h"

UCLASS()
class EMERGECORE_API UEmergeObjectiveTracker : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|ObjectiveTracker")
	static float Progress01(int32 Current, int32 Target);

	UFUNCTION(BlueprintCallable, Category = "Shell|ObjectiveTracker")
	static bool IsComplete(int32 Current, int32 Target);

	UFUNCTION(BlueprintCallable, Category = "Shell|ObjectiveTracker")
	static int32 Remaining(int32 Current, int32 Target);
};