#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStaminaHud.generated.h"

UCLASS()
class EMERGECORE_API UEmergeStaminaHud : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float BarFill01(float Current, float MaxStamina);

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static bool CanPerform(float Current, float Threshold);

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float JumpThreshold();

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float VaultThreshold();

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float ClimbThreshold();

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float HeavyMeleeThreshold();

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float RollThreshold();

	UFUNCTION(BlueprintCallable, Category = "Shell|StaminaHud")
	static float RegenCooldownSec(bool bExhausted);
};