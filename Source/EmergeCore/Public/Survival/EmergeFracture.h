#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFracture.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFracture : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Fracture")
	static float MoveSpeedMultiplier(bool bFractured, bool bSplinted);

	UFUNCTION(BlueprintCallable, Category = "Survival|Fracture")
	static bool CanSprint(bool bFractured, bool bSplinted);

	UFUNCTION(BlueprintCallable, Category = "Survival|Fracture")
	static float FractureHealPerTick(bool bSplinted, float DeltaT);
};