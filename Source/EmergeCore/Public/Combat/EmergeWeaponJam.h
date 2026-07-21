#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWeaponJam.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWeaponJam : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|WeaponJam")
	static float JamChance(float WeaponHealth01, float Heat01);

	UFUNCTION(BlueprintCallable, Category = "Combat|WeaponJam")
	static bool WouldJam(float JamRoll01, float Chance);
};