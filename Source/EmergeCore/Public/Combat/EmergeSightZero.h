#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSightZero.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSightZero : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|SightZero")
	static float HoldoverAtRange(float Distance, float ZeroDistance, float DropPerMeter);

	UFUNCTION(BlueprintCallable, Category = "Combat|SightZero")
	static bool IsZeroedFor(float Distance, float ZeroDistance, float Tolerance);
};