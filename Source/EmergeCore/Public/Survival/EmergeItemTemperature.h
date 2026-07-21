#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeItemTemperature.generated.h"

UCLASS()
class EMERGECORE_API UEmergeItemTemperature : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Item Temperature")
	static int32 TemperatureLevel(float TempC);

	UFUNCTION(BlueprintCallable, Category = "Survival|Item Temperature")
	static bool IsHot(float TempC);

	UFUNCTION(BlueprintCallable, Category = "Survival|Item Temperature")
	static bool IsFreezing(float TempC);
};