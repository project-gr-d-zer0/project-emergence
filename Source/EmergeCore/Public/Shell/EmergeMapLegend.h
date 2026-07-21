#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMapLegend.generated.h"

UCLASS()
class EMERGECORE_API UEmergeMapLegend : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|MapLegend")
	static int32 IconForType(int32 MarkerType);

	UFUNCTION(BlueprintCallable, Category = "Shell|MapLegend")
	static bool IsHostileMarker(int32 MarkerType);

	UFUNCTION(BlueprintCallable, Category = "Shell|MapLegend")
	static float IconPriority(int32 MarkerType);
};