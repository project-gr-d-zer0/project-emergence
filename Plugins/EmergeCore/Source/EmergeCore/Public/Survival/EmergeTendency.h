#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeTendency.generated.h"

UCLASS()
class EMERGECORE_API UEmergeTendency : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Tendency")
	static int32 TendencyLevel(float Delta);
};