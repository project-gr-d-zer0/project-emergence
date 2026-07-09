#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeNavPathLen.generated.h"

// Measures remaining path length along a waypoint polyline (straight-line distance under-reports around
// corners). Used alongside GetNavProgress to report accurate remaining distance / fraction traveled.
UCLASS()
class EMERGECORE_API UEmergeNavPathLen : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Sums Current->Points[FromIdx] then the remaining segment lengths through the polyline.
	UFUNCTION(BlueprintCallable, Category = "Nav")
	static float RemainingLength(const TArray<FVector>& Points, int32 FromIdx, FVector Current);

	// Fraction of TotalLength already traveled, clamped to [0,1]. TotalLength <= 0 -> 1.0 (done).
	UFUNCTION(BlueprintCallable, Category = "Nav")
	static float ProgressFraction(float TotalLength, float RemainingLength);
};
