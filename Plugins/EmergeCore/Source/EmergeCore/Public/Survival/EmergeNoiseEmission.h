#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNoiseEmission.generated.h"

UCLASS()
class EMERGECORE_API UEmergeNoiseEmission : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Noise")
	static float NoiseRadius(int32 Action);

	UFUNCTION(BlueprintCallable, Category = "Survival|Noise")
	static bool CanHear(float NoiseRadius, float ListenerDistance);
};