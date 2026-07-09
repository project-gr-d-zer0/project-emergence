#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNavFollow.generated.h"

// Deterministic path-following math for the NavigateTo harness (arrival test, waypoint advance, progress fraction).
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeNavFollow : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Nav")
	static bool ArrivedAt(float DistToGoal, float AcceptRadius);

	UFUNCTION(BlueprintCallable, Category = "Nav")
	static bool ShouldAdvanceWaypoint(float DistToWaypoint, float AdvanceRadius);

	UFUNCTION(BlueprintCallable, Category = "Nav")
	static float NavProgress(float TotalPathLen, float RemainingLen);
};
