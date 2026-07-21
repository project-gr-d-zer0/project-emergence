#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDigestion.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDigestion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Digestion")
	static float DigestionPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Digestion")
	static float SolidEmptiedPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Digestion")
	static float EnergyTransferPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Digestion")
	static float WaterTransferPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Digestion")
	static int32 FullnessLevel(float StomachVolume);

	UFUNCTION(BlueprintCallable, Category = "Survival|Digestion")
	static float AgentsTransferred(float DigestedAmount, float Digestibility);
};