#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeThrowableTimer.generated.h"

UCLASS()
class EMERGECORE_API UEmergeThrowableTimer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Throwable Timer")
	static float FuseRemaining(float FuseTime, float Cooked);

	UFUNCTION(BlueprintCallable, Category = "Combat|Throwable Timer")
	static bool HasDetonated(float FuseTime, float Cooked);

	UFUNCTION(BlueprintCallable, Category = "Combat|Throwable Timer")
	static bool DetonatesInAir(float FuseTime, float Cooked, float FlightTime);
};