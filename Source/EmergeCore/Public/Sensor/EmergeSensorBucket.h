#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSensorBucket.generated.h"

// Quantizes spatial telemetry for LLM consumption: bearing to 8-way sector, distance to
// near/med/far discrete buckets, instead of raw floats.
UCLASS()
class EMERGECORE_API UEmergeSensorBucket : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 8-way compass sector from a relative offset (RelX, RelY): 0=+X, 2=+Y, 4=-X, 6=-Y.
	UFUNCTION(BlueprintCallable, Category = "Sensor|Bucket")
	static int32 BearingSector(float RelX, float RelY);

	// Distance bucket: 0 = near (<300), 1 = med (<1000), 2 = far (>=1000).
	UFUNCTION(BlueprintCallable, Category = "Sensor|Bucket")
	static int32 DistanceBucket(float Dist);
};
