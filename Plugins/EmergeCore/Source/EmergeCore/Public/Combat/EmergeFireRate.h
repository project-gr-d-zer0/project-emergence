#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFireRate.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFireRate : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|FireRate")
	static float ShotIntervalSec(float Rpm);

	UFUNCTION(BlueprintCallable, Category = "Combat|FireRate")
	static int32 RoundsInWindow(float Rpm, float WindowSec);

	UFUNCTION(BlueprintCallable, Category = "Combat|FireRate")
	static bool CanFire(float LastShotTime, float Now, float Rpm);
};