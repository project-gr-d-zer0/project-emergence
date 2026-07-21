#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBleeding.generated.h"

UCLASS()
class EMERGECORE_API UEmergeBleeding : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Bleeding")
	static float FlowModifier(int32 Intensity);

	UFUNCTION(BlueprintCallable, Category = "Survival|Bleeding")
	static float BloodPressureScale(float Blood);

	UFUNCTION(BlueprintCallable, Category = "Survival|Bleeding")
	static float SourceBloodLossPerTick(int32 Intensity, float Blood, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Bleeding")
	static float TotalBloodLossPerTick(int32 NumSources, int32 Intensity, float Blood, float DeltaT);

	UFUNCTION(BlueprintPure, Category = "Survival|Bleeding")
	static int32 SourceDurationSec();

	UFUNCTION(BlueprintPure, Category = "Survival|Bleeding")
	static float BurnFlowModifier();
};