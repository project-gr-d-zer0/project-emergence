#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNavClassify.generated.h"

// Interprets UE nav-query results as clean telemetry booleans.
// ENavigationQueryResult: Invalid=0, Error=1, Fail=2, Success=3.
UCLASS()
class EMERGECORE_API UEmergeNavClassify : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// True only when the query result is Success (3) and the path is a complete (non-partial) path.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Nav")
	static bool IsReachable(int32 QueryResult, bool bIsPartial);

	// Ratio of actual path length to straight-line distance. Guards against a zero/negative
	// straight distance by returning 1.0 (no detour) in that degenerate case.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Nav")
	static float DetourRatio(float PathLength, float StraightDist);
};
