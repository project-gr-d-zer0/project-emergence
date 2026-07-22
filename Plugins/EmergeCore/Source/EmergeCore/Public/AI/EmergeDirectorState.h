#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDirectorState.generated.h"

// L4D-style director pacing state machine (research: Booth — BuildUp -> SustainPeak 4s -> PeakFade ->
// Relax 35s or 15000uu -> BuildUp). Deterministic, data-driven step function.
UCLASS()
class EMERGECORE_API UEmergeDirectorState : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// States: 0=BuildUp 1=SustainPeak 2=PeakFade 3=Relax.
	UFUNCTION(BlueprintCallable, Category = "AI|Director")
	static int32 Step(int32 State, float Intensity, float TimeInState, float DistTraveledUu);
};
