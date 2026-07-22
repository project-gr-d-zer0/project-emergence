#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeReticle.generated.h"

UCLASS()
class EMERGECORE_API UEmergeReticle : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Reticle")
	static int32 ReticleForOptic(int32 OpticType);

	UFUNCTION(BlueprintCallable, Category = "Shell|Reticle")
	static bool HasRangefinder(int32 OpticType);

	UFUNCTION(BlueprintCallable, Category = "Shell|Reticle")
	static float CenterDotSize(int32 OpticType);
};
