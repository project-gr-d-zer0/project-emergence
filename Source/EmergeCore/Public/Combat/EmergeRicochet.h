#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeRicochet.generated.h"

UCLASS()
class EMERGECORE_API UEmergeRicochet : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Ricochet")
	static bool WouldRicochet(float ImpactAngleDeg, float RicochetThreshold);

	UFUNCTION(BlueprintCallable, Category = "Combat|Ricochet")
	static float RetainedVelocity01(float ImpactAngleDeg, float RicochetThreshold);
};