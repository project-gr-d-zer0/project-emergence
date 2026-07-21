#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeGrenadeBlast.generated.h"

UCLASS()
class EMERGECORE_API UEmergeGrenadeBlast : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Grenade")
	static float DamageAtDistance(float MaxDamage, float Distance, float BlastRadius);

	UFUNCTION(BlueprintCallable, Category = "Combat|Grenade")
	static bool InLethalRange(float Distance, float LethalRadius);
};