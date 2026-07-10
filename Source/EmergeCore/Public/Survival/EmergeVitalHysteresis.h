#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeVitalHysteresis.generated.h"

// Vital severity tiers with hysteresis: worsening is immediate, recovery requires clearing the
// better tier's floor + 5 (prevents threshold flapping at exact band edges).
UCLASS()
class EMERGECORE_API UEmergeVitalHysteresis : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Raw severity band for a value: >=75 tier0, 50..74 tier1, 25..49 tier2, <25 tier3.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival")
	static int32 RawTier(float Value);

	// Advances CurrentTier toward the raw tier for Value, applying recovery hysteresis.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival")
	static int32 TierStep(int32 CurrentTier, float Value);
};
