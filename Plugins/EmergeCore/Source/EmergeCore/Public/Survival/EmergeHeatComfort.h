#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHeatComfort.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHeatComfort : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Determines the heat comfort zone based on the heat comfort value.
	 * @param HeatComfort The heat comfort value in [-1, +1] range
	 * @return Signed zone: hot +1/+2/+3 (warning/critical/extreme), cold -1/-2/-3, 0 = comfortable
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Survival")
	static int32 ComfortZone(float HeatComfort);

	/**
	 * Moves the current heat comfort value toward the target by a clamped step.
	 * @param Current The current heat comfort value
	 * @param Target The target heat comfort value
	 * @return The new heat comfort value after moving toward the target
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Survival")
	static float StepTowards(float Current, float Target);

	/**
	 * Gets the weight contribution of a specific body part slot to the final heat comfort.
	 * @param Slot The body part slot index
	 * @return The weight contribution for the specified slot
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Survival")
	static float BodyPartWeight(int32 Slot);
};
