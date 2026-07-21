#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDurabilityBar.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDurabilityBar : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Durability")
	static float Fill01(float Current, float Max);

	UFUNCTION(BlueprintCallable, Category = "Shell|Durability")
	static int32 ColorTier(float Fill01);

	UFUNCTION(BlueprintCallable, Category = "Shell|Durability")
	static bool IsBroken(float Current);
};