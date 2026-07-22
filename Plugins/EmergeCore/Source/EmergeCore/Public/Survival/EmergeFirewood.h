#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFirewood.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFirewood : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Firewood")
	static int32 YieldForTool(int32 ToolTier);

	UFUNCTION(BlueprintCallable, Category = "Survival|Firewood")
	static float GatherTimeSec(int32 ToolTier);

	UFUNCTION(BlueprintCallable, Category = "Survival|Firewood")
	static float BurnValuePerLog();
};