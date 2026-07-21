#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSleepRecovery.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSleepRecovery : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|SleepRecovery")
	static float EnergyRestorePerTick(int32 RestState, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|SleepRecovery")
	static float FatigueRecoverPerTick(int32 RestState, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|SleepRecovery")
	static bool IsResting(int32 RestState);
};