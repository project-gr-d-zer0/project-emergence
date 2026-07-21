#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSuppressor.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSuppressor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Suppressor")
	static float NoiseRadius(float BaseRadius, bool bSuppressed);

	UFUNCTION(BlueprintCallable, Category = "Combat|Suppressor")
	static float MuzzleFlashScale(bool bSuppressed);
};