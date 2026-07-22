#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeRopeClimb.generated.h"

UCLASS()
class EMERGECORE_API UEmergeRopeClimb : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Survival|RopeClimb")
    static float StaminaCost(float HeightMeters, float PerMeterCost);

    UFUNCTION(BlueprintCallable, Category = "Survival|RopeClimb")
    static bool CanClimb(float CurrentStamina, float HeightMeters, float PerMeterCost);

    UFUNCTION(BlueprintCallable, Category = "Survival|RopeClimb")
    static float ClimbTimeSec(float HeightMeters, float ClimbSpeedMps);
};