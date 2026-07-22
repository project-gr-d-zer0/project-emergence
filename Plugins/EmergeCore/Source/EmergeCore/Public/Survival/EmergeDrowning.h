#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDrowning.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDrowning : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Drowning")
	static float SwimmingThreshold();

	UFUNCTION(BlueprintCallable, Category = "Survival|Drowning")
	static float UnconsciousThreshold();

	UFUNCTION(BlueprintCallable, Category = "Survival|Drowning")
	static bool IsDrowning(float Submersion01, float Threshold);

	UFUNCTION(BlueprintCallable, Category = "Survival|Drowning")
	static float BubbleIntervalMs(float Stamina01);
};