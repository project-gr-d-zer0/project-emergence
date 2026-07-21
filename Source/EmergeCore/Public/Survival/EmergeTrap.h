#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeTrap.generated.h"

UCLASS()
class EMERGECORE_API UEmergeTrap : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Trap")
	static float CatchChancePerCheck(int32 TrapType, bool bBaited);

	UFUNCTION(BlueprintCallable, Category = "Survival|Trap")
	static bool Caught(float Roll01, float Chance);

	UFUNCTION(BlueprintCallable, Category = "Survival|Trap")
	static bool IsValidTrap(int32 TrapType);
};