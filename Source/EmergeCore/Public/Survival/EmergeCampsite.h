#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCampsite.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCampsite : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Campsite")
	static float SafetyScore(bool bSheltered, bool bNearWater, int32 ThreatLevel);

	UFUNCTION(BlueprintCallable, Category = "Survival|Campsite")
	static bool IsSafe(float SafetyScore);
};