#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeShelter.generated.h"

UCLASS()
class EMERGECORE_API UEmergeShelter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Shelter")
	static bool CanBuild(int32 MaterialsHave, int32 MaterialsReq);

	UFUNCTION(BlueprintCallable, Category = "Survival|Shelter")
	static float BuildProgressPerTick(bool bBuilding, float BuildSpeed, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Shelter")
	static bool IsComplete(float Progress, float Required);
};