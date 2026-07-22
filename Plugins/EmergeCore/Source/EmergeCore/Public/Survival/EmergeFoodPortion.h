#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFoodPortion.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFoodPortion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|FoodPortion")
	static float PortionSize(float TotalAmount, int32 Portions);

	UFUNCTION(BlueprintCallable, Category = "Survival|FoodPortion")
	static int32 MaxPortions(float TotalAmount, float MinPortion);

	UFUNCTION(BlueprintCallable, Category = "Survival|FoodPortion")
	static float Remaining(float TotalAmount, float Consumed);
};