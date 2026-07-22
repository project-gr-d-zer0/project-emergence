#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCompassBearing.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCompassBearing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|CompassBearing")
	static float RelativeBearingDeg(float TargetBearing, float FacingBearing);

	UFUNCTION(BlueprintCallable, Category = "Shell|CompassBearing")
	static float MarkerOffset(float RelativeBearingDeg, float FovDeg);

	UFUNCTION(BlueprintCallable, Category = "Shell|CompassBearing")
	static bool IsInView(float RelativeBearingDeg, float FovDeg);
};