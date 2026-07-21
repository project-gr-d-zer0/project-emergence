#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSaltPreserve.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSaltPreserve : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|SaltPreserve")
	static float SpoilRateMultiplier(bool bSalted);

	UFUNCTION(BlueprintCallable, Category = "Survival|SaltPreserve")
	static float PreservedSpoilPerTick(float BaseSpoilRate, bool bSalted, float DeltaT);
};