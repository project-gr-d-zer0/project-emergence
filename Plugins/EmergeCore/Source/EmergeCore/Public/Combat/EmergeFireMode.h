#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFireMode.generated.h"

UCLASS()
class EMERGECORE_API UEmergeFireMode : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|FireMode")
	static int32 NextMode(int32 Current, int32 MaxMode);

	UFUNCTION(BlueprintCallable, Category = "Combat|FireMode")
	static int32 RoundsPerPull(int32 Mode);

	UFUNCTION(BlueprintCallable, Category = "Combat|FireMode")
	static bool CanFire(int32 Mode);
};