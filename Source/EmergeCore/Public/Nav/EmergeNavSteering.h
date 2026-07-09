#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNavSteering.generated.h"

// Pure steering math: heading to a target, turn error vs current facing, and a slow-for-turn gate.
UCLASS()
class EMERGECORE_API UEmergeNavSteering : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Nav|Steering")
	static FVector2D DesiredHeading(FVector2D From, FVector2D To);

	UFUNCTION(BlueprintCallable, Category = "Nav|Steering")
	static float TurnErrorDeg(FVector2D Heading, FVector2D Facing);

	UFUNCTION(BlueprintCallable, Category = "Nav|Steering")
	static bool ShouldSlowForTurn(float TurnErrorDeg, float ThresholdDeg);
};
