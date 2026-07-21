#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWeaponSwap.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWeaponSwap : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|WeaponSwap")
	static float SwapTimeSec(float BaseTime, float WeaponKg);

	UFUNCTION(BlueprintCallable, Category = "Combat|WeaponSwap")
	static float FastSwapTimeSec(float BaseTime, float WeaponKg);

	UFUNCTION(BlueprintCallable, Category = "Combat|WeaponSwap")
	static bool CanSwap(bool bReloading);
};