#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNavRecovery.generated.h"

// Deterministic decision ladder for a stuck path-follower.
// 0=Continue, 1=Repath, 3=Abandon.
UCLASS()
class EMERGECORE_API UEmergeNavRecovery : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Nav|Recovery")
	static int32 DecideRecovery(float StuckSeconds, int32 RepathCount, float StuckThreshold, int32 MaxRepaths);

	UFUNCTION(BlueprintCallable, Category = "Nav|Recovery")
	static bool ShouldGiveUp(int32 ActionCode);
};
