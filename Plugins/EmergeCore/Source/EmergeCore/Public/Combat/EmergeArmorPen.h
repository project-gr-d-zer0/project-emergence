#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeArmorPen.generated.h"

UCLASS()
class EMERGECORE_API UEmergeArmorPen : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Armor Penetration")
	static float RequiredPen(int32 ArmorClass);

	UFUNCTION(BlueprintCallable, Category = "Combat|Armor Penetration")
	static bool Penetrates(float PenPower, int32 ArmorClass);

	UFUNCTION(BlueprintCallable, Category = "Combat|Armor Penetration")
	static float DamageMultiplier(float PenPower, int32 ArmorClass);
};