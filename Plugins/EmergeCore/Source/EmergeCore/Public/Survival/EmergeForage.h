#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeForage.generated.h"

UCLASS()
class EMERGECORE_API UEmergeForage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Forage")
	static int32 YieldForArea(int32 Richness, bool bTool);

	UFUNCTION(BlueprintCallable, Category = "Survival|Forage")
	static float RegrowTimeSec(int32 Richness);

	UFUNCTION(BlueprintCallable, Category = "Survival|Forage")
	static bool IsForageable(int32 Richness);
};