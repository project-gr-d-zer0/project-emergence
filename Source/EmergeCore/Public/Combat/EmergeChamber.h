#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeChamber.generated.h"

UCLASS()
class EMERGECORE_API UEmergeChamber : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Chamber")
	static int32 TotalWithChamber(int32 MagRounds, bool bChambered);

	UFUNCTION(BlueprintCallable, Category = "Combat|Chamber")
	static int32 MaxCapacity(int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Combat|Chamber")
	static bool CanChamber(int32 MagRounds, bool bAlreadyChambered);
};