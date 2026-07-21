#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCampfire.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCampfire : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Campfire")
	static float FuelBurnedPerTick(bool bLit, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Campfire")
	static float BurnTimeRemaining(float Fuel, float BurnRate);

	UFUNCTION(BlueprintCallable, Category = "Survival|Campfire")
	static bool IsLit(float Fuel);
};