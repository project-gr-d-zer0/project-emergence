#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFlashlightBlind.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFlashlightBlind : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Combat|Flashlight")
    static float BlindIntensity(float Distance, float MaxRange, bool bFacing);

    UFUNCTION(BlueprintCallable, Category = "Combat|Flashlight")
    static bool IsBlinded(float Intensity, float Threshold);
};