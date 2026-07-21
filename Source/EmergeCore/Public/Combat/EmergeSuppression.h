#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSuppression.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSuppression : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Suppression")
	static float SuppressionLevel(float MissDistance, float MaxSuppressRange);

	UFUNCTION(BlueprintCallable, Category = "Combat|Suppression")
	static float AimPenalty(float SuppressionLevel, float MaxPenalty);

	UFUNCTION(BlueprintCallable, Category = "Combat|Suppression")
	static float DecayPerTick(float Current, float DecayRate, float DeltaT);
};