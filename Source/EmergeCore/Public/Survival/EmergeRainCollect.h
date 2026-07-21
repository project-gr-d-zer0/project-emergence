#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeRainCollect.generated.h"

UCLASS()
class EMERGECORE_API UEmergeRainCollect : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|RainCollect")
	static float CollectRatePerTick(int32 Weather, float ContainerArea, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|RainCollect")
	static float FilledAfter(float Current, float Rate, float Seconds, float Capacity);
};