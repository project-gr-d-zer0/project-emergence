#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAmmoType.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAmmoType : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoType")
	static bool IsArmorPiercing(int32 AmmoType);

	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoType")
	static int32 IconColor(int32 AmmoType);

	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoType")
	static float DamageModifier(int32 AmmoType);
};