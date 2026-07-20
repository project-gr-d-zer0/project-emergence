#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeFoodSpoilage.generated.h"

// Food spoilage math (Project Zomboid B41 Food.java parity). Age accrues in days toward
// Stale (OffAge) then Rotten (OffAgeMax). Cold storage only helps while powered, tying this
// directly to the utility grid timeline.
UCLASS()
class EMERGECORE_API UEmergeFoodSpoilage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 0 = Fresh, 1 = Stale, 2 = Rotten.
	UFUNCTION(BlueprintCallable, Category = "Survival|FoodSpoilage")
	static int32 FreshnessState(float Age, float OffAge, float OffAgeMax);

	// PZ FoodRotSpeed sandbox multiplier: presets 1/2/4/5 -> 1.7/1.4/0.7/0.4; else 1.0 (preset 3 default).
	UFUNCTION(BlueprintCallable, Category = "Survival|FoodSpoilage")
	static float RotSpeedMult(int32 FoodRotSpeedPreset);

	// StorageKind: 0 = Room, 1 = Fridge, 2 = Freezer. Cold storage only helps while powered.
	UFUNCTION(BlueprintCallable, Category = "Survival|FoodSpoilage")
	static float StorageRateMult(int32 StorageKind, bool bPowered);

	// ElapsedHours * StorageRateMult * RotSpeedMult / 24.
	UFUNCTION(BlueprintCallable, Category = "Survival|FoodSpoilage")
	static float AgeDeltaDays(float ElapsedHours, int32 StorageKind, bool bPowered, int32 FoodRotSpeedPreset);
};
