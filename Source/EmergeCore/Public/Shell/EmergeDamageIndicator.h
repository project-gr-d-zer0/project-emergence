#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDamageIndicator.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDamageIndicator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|DamageIndicator")
	static float RelativeBearing(float HitDirDeg, float FacingDeg);

	UFUNCTION(BlueprintCallable, Category = "Shell|DamageIndicator")
	static int32 Sector8(float RelativeBearing);

	UFUNCTION(BlueprintCallable, Category = "Shell|DamageIndicator")
	static float Intensity01(float Distance, float MaxDistance);
};