#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHealthVignette.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHealthVignette : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|HealthVignette")
	static float VignetteIntensity(float Health01);

	UFUNCTION(BlueprintCallable, Category = "Shell|HealthVignette")
	static float Desaturation01(float Health01);

	UFUNCTION(BlueprintCallable, Category = "Shell|HealthVignette")
	static bool IsCriticalPulse(float Health01);
};