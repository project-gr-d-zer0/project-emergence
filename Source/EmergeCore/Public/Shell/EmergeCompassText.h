#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCompassText.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCompassText : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Compass")
	static int32 DegreesInt(float HeadingDeg);

	UFUNCTION(BlueprintCallable, Category = "Shell|Compass")
	static float ToMils(float HeadingDeg);
};