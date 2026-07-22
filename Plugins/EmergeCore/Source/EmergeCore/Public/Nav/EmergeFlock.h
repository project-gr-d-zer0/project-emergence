#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFlock.generated.h"

// Pure Reynolds boid steering math: separation, cohesion, alignment, and a weighted combine.
UCLASS()
class EMERGECORE_API UEmergeFlock : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Nav|Flock")
	static FVector Separation(FVector SelfLocation, FVector Neighbor, float DesiredDist);

	UFUNCTION(BlueprintCallable, Category = "Nav|Flock")
	static FVector Cohesion(FVector SelfLocation, FVector GroupCenter);

	UFUNCTION(BlueprintCallable, Category = "Nav|Flock")
	static FVector Alignment(FVector AvgHeading);

	UFUNCTION(BlueprintCallable, Category = "Nav|Flock")
	static FVector Combine(FVector Sep, FVector Coh, FVector Ali, FVector Seek, float WSep, float WCoh, float WAli, float WSeek);
};
