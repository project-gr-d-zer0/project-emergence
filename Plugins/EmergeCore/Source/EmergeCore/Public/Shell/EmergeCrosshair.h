#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCrosshair.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCrosshair : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Crosshair")
	static float SpreadGap(float BaseGap, float MoveSpeed01, bool bFiring);

	UFUNCTION(BlueprintCallable, Category = "Shell|Crosshair")
	static float RecoverGap(float Current, float BaseGap, float RecoverRate, float DeltaT);
};