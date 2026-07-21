#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFallDamage.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFallDamage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Fall Damage")
	static float DamageFraction(float HeightM, float LowM, float HighM);

	UFUNCTION(BlueprintCallable, Category = "Fall Damage")
	static float HealthDamageFraction(float HeightM);

	UFUNCTION(BlueprintCallable, Category = "Fall Damage")
	static float ShockDamageFraction(float HeightM);

	UFUNCTION(BlueprintCallable, Category = "Fall Damage")
	static bool CanBreakLegs(float HeightM);

	UFUNCTION(BlueprintCallable, Category = "Fall Damage")
	static bool IsSafeFall(float HeightM);
};