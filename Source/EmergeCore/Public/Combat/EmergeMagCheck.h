#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMagCheck.generated.h"

UCLASS()
class EMERGECORE_API UEmergeMagCheck : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Magazine")
	static int32 AmmoBand(int32 Current, int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Combat|Magazine")
	static bool NeedsReload(int32 Current, int32 MagSize);
};