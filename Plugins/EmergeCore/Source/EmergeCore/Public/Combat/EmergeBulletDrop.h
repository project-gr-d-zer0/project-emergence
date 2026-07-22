#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBulletDrop.generated.h"

UCLASS()
class EMERGECORE_API UEmergeBulletDrop : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Combat|Ballistics")
    static float FlightTime(float Distance, float Velocity);

    UFUNCTION(BlueprintCallable, Category = "Combat|Ballistics")
    static float Drop(float Distance, float Velocity, float Gravity);
};