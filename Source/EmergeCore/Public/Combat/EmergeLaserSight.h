#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeLaserSight.generated.h"

UCLASS()
class EMERGECORE_API UEmergeLaserSight : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|LaserSight")
	static float HipfireSpreadMultiplier(bool bLaserOn);

	UFUNCTION(BlueprintCallable, Category = "Combat|LaserSight")
	static bool IsVisibleToEnemy(float Distance, float MaxVisRange, bool bLaserOn);
};