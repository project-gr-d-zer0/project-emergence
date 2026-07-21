#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMuzzleBrake.generated.h"

UCLASS()
class EMERGECORE_API UEmergeMuzzleBrake : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|MuzzleBrake")
	static float RecoilMultiplier(bool bBrake);

	UFUNCTION(BlueprintCallable, Category = "Combat|MuzzleBrake")
	static float SideBlastMultiplier(bool bBrake);
};