#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCookState.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCookState : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Cooking")
	static int32 CookStage(float CookProgress, float CookedAt, float BurnedAt);

	UFUNCTION(BlueprintCallable, Category = "Survival|Cooking")
	static float CookProgressPerTick(bool bOnHeat, float DeltaT);
};