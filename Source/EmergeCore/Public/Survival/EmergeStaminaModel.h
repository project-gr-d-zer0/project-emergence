#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStaminaModel.generated.h"

UCLASS()
class EMERGECORE_API UEmergeStaminaModel : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival|Stamina")
	static float MaxStaminaForLoad(float LoadKg);

	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival|Stamina")
	static float SprintDrainPerSec(int32 Stance);

	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival|Stamina")
	static float ActionDrain(int32 Action);

	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival|Stamina")
	static float RegenPerSec(int32 MoveState);
};