#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMedical.generated.h"

// Pure, deterministic bleeding/healing math (ties Combat -> Survival).
UCLASS()
class EMERGECORE_API UEmergeMedical : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Damage per tick from bleeding; zero when not bleeding.
	UFUNCTION(BlueprintCallable, Category = "Medical")
	static float BleedDamage(float BleedRate, float DeltaSeconds, bool bBleeding);

	// Applies heal amount to current HP, clamped to [0, 100].
	UFUNCTION(BlueprintCallable, Category = "Medical")
	static float ApplyHeal(float CurrentHP, float HealAmount);

	// Bandage (1) and medkit (2) stop bleeding; anything else does not.
	UFUNCTION(BlueprintPure, Category = "Medical")
	static bool StopsBleeding(int32 ItemType);
};
