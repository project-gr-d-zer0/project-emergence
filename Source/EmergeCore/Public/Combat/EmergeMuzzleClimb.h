#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMuzzleClimb.generated.h"

UCLASS()
class EMERGECORE_API UEmergeMuzzleClimb : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|MuzzleClimb")
	static float ClimbAtShot(int32 ShotIndex, float ClimbPerShot, float GripControl01, float MaxClimb);

	UFUNCTION(BlueprintCallable, Category = "Combat|MuzzleClimb")
	static float SettlePerTick(float Current, float SettleRate, float DeltaT);
};