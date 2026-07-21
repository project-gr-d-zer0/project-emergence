#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSaline.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSaline : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Saline")
	static float BloodRegenPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Saline")
	static float WaterRegenPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Saline")
	static float LiquidAmount();

	UFUNCTION(BlueprintCallable, Category = "Survival|Saline")
	static float BloodRestoredOver(float Seconds);
};