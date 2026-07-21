#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCombatResolve.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCombatResolve : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Resolve")
	static float ResolveDamage(float BaseDamage, float Distance, float FalloffStart, float FalloffEnd,
		float MinMult, int32 AmmoType, float PenPower, int32 ArmorClass, int32 HitZone);
};