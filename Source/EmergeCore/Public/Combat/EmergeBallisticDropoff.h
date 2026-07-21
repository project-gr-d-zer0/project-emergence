#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBallisticDropoff.generated.h"

UCLASS()
class EMERGECORE_API UEmergeBallisticDropoff : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Ballistics")
	static float DamageAtRange(float BaseDamage, float Distance, float FalloffStart, float FalloffEnd, float MinMultiplier);

	UFUNCTION(BlueprintCallable, Category = "Combat|Ballistics")
	static float VelocityAtRange(float MuzzleVel, float Distance, float DragPerMeter);
};