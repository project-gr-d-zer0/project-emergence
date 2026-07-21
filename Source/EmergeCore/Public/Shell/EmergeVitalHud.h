#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeVitalHud.generated.h"

UCLASS()
class EMERGECORE_API UEmergeVitalHud : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static float BarFill01(float Value, float Max);

	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static int32 BloodStatLevel(float Blood);

	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static int32 HealthStatLevel(float Health);

	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static int32 EnergyStatLevel(float Energy);

	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static int32 WaterStatLevel(float Water);

	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static int32 DisplayColorTier(int32 StatLevel);

	UFUNCTION(BlueprintCallable, Category = "Shell|VitalHud")
	static bool ShowLowVitalBadge(int32 StatLevel);
};