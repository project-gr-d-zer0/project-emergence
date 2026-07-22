#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAimSway.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAimSway : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|AimSway")
	static float SwayAmplitude(float BaseSway, bool bHoldingBreath, float Stamina01);

	UFUNCTION(BlueprintCallable, Category = "Combat|AimSway")
	static bool CanHoldBreath(float Stamina01);
};