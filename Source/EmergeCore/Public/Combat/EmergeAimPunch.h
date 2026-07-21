#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAimPunch.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAimPunch : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|AimPunch")
	static float PunchMagnitude(float Damage, float PunchPerDamage, float MaxPunch);

	UFUNCTION(BlueprintCallable, Category = "Combat|AimPunch")
	static float RecoverPunch(float Current, float RecoverRate, float DeltaT);
};