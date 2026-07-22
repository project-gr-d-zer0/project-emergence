#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeInfluence.generated.h"

// Deterministic influence-map math (threat/friendly fields for the sensor world-model).
UCLASS()
class EMERGECORE_API UEmergeInfluence : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Distance-falloff stamp: full strength at Dist 0, zero at/after MaxDist, curved by CurvePow.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Influence")
	static float StampFalloff(float Strength, float Dist, float MaxDist, int32 CurvePow);

	// Combined tension of threat and friendly influence.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Influence")
	static float Tension(float Threat, float Friendly);

	// Net control: friendly minus threat.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Influence")
	static float NetControl(float Friendly, float Threat);
};
