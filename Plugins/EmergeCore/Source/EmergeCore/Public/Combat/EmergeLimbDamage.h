#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeLimbDamage.generated.h"

// Limb damage multipliers + leg-injury locomotion tiers (Tarkov limb model research).
UCLASS()
class EMERGECORE_API UEmergeLimbDamage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 0 head->2.5, 1 thorax->1.0, 2 stomach->1.2, 3 arm->0.7, 4 leg->0.65, else 1.0
	UFUNCTION(BlueprintPure, Category = "Emerge|Combat")
	static float MultiplierForLimb(int32 Limb);

	// 0 fine->1.0, 1 bruised->0.85, 2 fractured->0.6, 3 and beyond blacked->0.4
	UFUNCTION(BlueprintPure, Category = "Emerge|Combat")
	static float SpeedMultiplierForLegState(int32 State);

	// Sprint allowed only while state < 2 (fine/bruised).
	UFUNCTION(BlueprintPure, Category = "Emerge|Combat")
	static bool CanSprintWithLegState(int32 State);
};
