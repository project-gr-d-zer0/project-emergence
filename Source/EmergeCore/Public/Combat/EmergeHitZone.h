#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHitZone.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHitZone : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|HitZone")
	static float ZoneMultiplier(int32 Zone);

	UFUNCTION(BlueprintCallable, Category = "Combat|HitZone")
	static float ApplyZone(float BaseDamage, int32 Zone);
};