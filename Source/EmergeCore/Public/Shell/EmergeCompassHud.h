#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCompassHud.generated.h"

UCLASS()
class EMERGECORE_API UEmergeCompassHud : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Compass")
	static float NormalizeHeading(float Degrees);
	
	UFUNCTION(BlueprintCallable, Category = "Shell|Compass")
	static int32 Cardinal8(float Degrees);
};