#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHitmarker.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHitmarker : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Hitmarker")
	static int32 MarkerState(bool bHit, bool bArmor, bool bHeadshot, bool bKill);

	UFUNCTION(BlueprintCallable, Category = "Shell|Hitmarker")
	static float MarkerScale(int32 State);
};