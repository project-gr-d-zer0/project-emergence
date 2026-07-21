#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDamagePopup.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDamagePopup : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Emergence|Shell")
	static float PopupScale(float Damage, float RefDamage);

	UFUNCTION(BlueprintCallable, Category = "Emergence|Shell")
	static int32 SeverityColor(float Damage, float RefDamage);

	UFUNCTION(BlueprintCallable, Category = "Emergence|Shell")
	static bool IsCrit(float Damage, float RefDamage);
};