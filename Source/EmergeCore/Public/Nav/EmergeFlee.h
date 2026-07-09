#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFlee.generated.h"

// Evasion math: fleeing a threat. Deterministic, unit-testable.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeFlee : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Nav|Flee")
	static FVector FleeDirection(FVector SelfLocation, FVector Threat);

	UFUNCTION(BlueprintCallable, Category = "Nav|Flee")
	static FVector FleePoint(FVector SelfLocation, FVector Threat, float Distance);

	UFUNCTION(BlueprintCallable, Category = "Nav|Flee")
	static float ThreatLevel(float Dist, float DangerRadius);
};
