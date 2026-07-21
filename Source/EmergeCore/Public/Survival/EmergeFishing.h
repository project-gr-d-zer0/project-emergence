#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFishing.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFishing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Fishing")
	static float BiteChancePerTick(int32 SpotQuality, bool bBaited, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Fishing")
	static bool GotBite(float Roll01, float Chance);
};