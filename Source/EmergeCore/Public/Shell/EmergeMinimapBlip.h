#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMinimapBlip.generated.h"

UCLASS()
class EMERGECORE_API UEmergeMinimapBlip : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Minimap")
	static float WorldToMap01(float WorldCoord, float WorldMin, float WorldMax);

	UFUNCTION(BlueprintCallable, Category = "Shell|Minimap")
	static bool IsOnMap(float WorldCoord, float WorldMin, float WorldMax);
};