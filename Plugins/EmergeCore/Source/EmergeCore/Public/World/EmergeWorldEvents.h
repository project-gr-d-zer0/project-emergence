#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWorldEvents.generated.h"

// Persistent world raid-timer substitute: horde SURGE events, shelter gating, and
// loot-driven "hunted" visibility broadcast. Pure, deterministic formulas.
UCLASS()
class EMERGECORE_API UEmergeWorldEvents : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Surge temporarily shifts effective depth deeper (meaner) by 2, reusing depth-difficulty kernels.
	UFUNCTION(BlueprintCallable, Category = "World|Events")
	static int32 SurgeDepthShift(int32 BaseDepth, bool bSurgeActive);

	// Shelter check gates surge lethality: indoors or underground counts as sheltered.
	UFUNCTION(BlueprintCallable, Category = "World|Events")
	static bool IsSheltered(bool bIndoors, bool bUnderground);

	// More unsecured loot value carried = more map-visible ("hunted"), Hunt-style broadcast.
	UFUNCTION(BlueprintCallable, Category = "World|Events")
	static float BroadcastVisibilityWeight(int32 ValueAtRisk);
};
