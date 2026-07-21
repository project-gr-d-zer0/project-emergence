#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeReloadTime.generated.h"

UCLASS()
class EMERGECORE_API UEmergeReloadTime : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Reload")
	static float ReloadTimeSec(float BaseTime, float Ergonomics01, bool bEmptyReload);
};