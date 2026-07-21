#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSurvivorTick.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSurvivorTick : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Tick")
	static float DrainedEnergy(float Energy, int32 Movement, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Tick")
	static float DrainedBlood(float Blood, int32 Intensity, int32 Sources, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Tick")
	static int32 ImmunityFromState(float Energy, float Water, float Health01, float Blood01);

	UFUNCTION(BlueprintCallable, Category = "Survival|Tick")
	static bool IsDeteriorating(float EnergyBefore, float EnergyAfter, float BloodBefore, float BloodAfter);
};