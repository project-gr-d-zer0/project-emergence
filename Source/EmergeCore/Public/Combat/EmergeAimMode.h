#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAimMode.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAimMode : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|AimMode")
	static float AccuracyMultiplier(bool bAds);

	UFUNCTION(BlueprintCallable, Category = "Combat|AimMode")
	static float MoveSpeedMultiplier(bool bAds);
};