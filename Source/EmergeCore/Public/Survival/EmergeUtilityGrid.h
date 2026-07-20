#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeUtilityGrid.generated.h"

// Municipal utility shutoff timeline (Project Zomboid water/power shutoff). Water and electricity
// run for a fixed number of in-game days, then cut permanently, forcing the survival economy.
UCLASS()
class EMERGECORE_API UEmergeUtilityGrid : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// A negative ShutoffDay means the utility never shuts off (infinite).
	UFUNCTION(BlueprintCallable, Category = "Survival|Utility")
	static bool IsUtilityOn(int32 ShutoffDay, int32 NightsSurvived);

	// UtilityKind: 0 = Water, 1 = Electricity. Returns (Min,Max) inclusive day range for the preset.
	// Preset outside 2..8 returns (-1,-1) (never shuts off).
	UFUNCTION(BlueprintCallable, Category = "Survival|Utility")
	static FIntPoint ShutoffDayRange(int32 UtilityKind, int32 Preset);

	// Deterministically pick a shutoff day inside the preset range. Roll01 is clamped to [0,1].
	UFUNCTION(BlueprintCallable, Category = "Survival|Utility")
	static int32 ResolveShutoffDay(int32 UtilityKind, int32 Preset, float Roll01);
};
