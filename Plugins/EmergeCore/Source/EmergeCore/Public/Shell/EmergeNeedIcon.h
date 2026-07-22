#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNeedIcon.generated.h"

UCLASS()
class EMERGECORE_API UEmergeNeedIcon : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|NeedIcon")
	static int32 IconState(float Value01);

	UFUNCTION(BlueprintCallable, Category = "Shell|NeedIcon")
	static bool ShouldFlash(float Value01);
};