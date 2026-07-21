#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeForegrip.generated.h"

UCLASS()
class EMERGECORE_API UEmergeForegrip : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Foregrip")
	static float RecoilMultiplier(int32 GripType);

	UFUNCTION(BlueprintCallable, Category = "Combat|Foregrip")
	static float HandlingMultiplier(int32 GripType);
};