#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStanceIndicator.generated.h"

UCLASS()
class EMERGECORE_API UEmergeStanceIndicator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|StanceIndicator")
	static float EyeHeightCm(int32 Stance);

	UFUNCTION(BlueprintCallable, Category = "Shell|StanceIndicator")
	static float MoveSpeedMultiplier(int32 Stance);
};