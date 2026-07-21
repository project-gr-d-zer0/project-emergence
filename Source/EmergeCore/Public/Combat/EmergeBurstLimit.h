#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBurstLimit.generated.h"

UCLASS()
class EMERGECORE_API UEmergeBurstLimit : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|BurstLimit")
	static int32 RoundsThisBurst(int32 TriggerHeldRounds, int32 BurstSize);

	UFUNCTION(BlueprintCallable, Category = "Combat|BurstLimit")
	static bool BurstComplete(int32 Fired, int32 BurstSize);

	UFUNCTION(BlueprintCallable, Category = "Combat|BurstLimit")
	static int32 RemainingInBurst(int32 Fired, int32 BurstSize);
};