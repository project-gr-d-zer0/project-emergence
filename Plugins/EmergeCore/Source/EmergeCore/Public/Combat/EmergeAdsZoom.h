#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAdsZoom.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAdsZoom : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|ADS")
	static float ZoomedFov(float BaseFov, float Magnification);

	UFUNCTION(BlueprintCallable, Category = "Combat|ADS")
	static float SensitivityMultiplier(float Magnification);
};