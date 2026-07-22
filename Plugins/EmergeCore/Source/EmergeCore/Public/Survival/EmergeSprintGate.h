#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSprintGate.generated.h"

// Sprint-exhaustion gate with hysteresis: exhausted at 0 stamina, stays denied until stamina
// recovers past a re-arm threshold (prevents sprint-flapping right at 0).
UCLASS()
class EMERGECORE_API UEmergeSprintGate : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Emerge|Survival")
	static bool UpdateExhausted(float Stamina, bool bWasExhausted, float ReArmThreshold);
};
