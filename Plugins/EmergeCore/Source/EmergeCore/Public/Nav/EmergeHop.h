#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHop.generated.h"

// Traversal hop launch math for non-ALS AI (e.g. zombies) clearing low obstacles
// with a parabolic LaunchCharacter hop. Deterministic, unit-testable headless.
UCLASS()
class EMERGECORE_API UEmergeHop : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Vertical velocity needed to top an obstacle of ObstacleHeightUu with ClearanceMarginUu of margin,
	// given GravityZ (may be negative or positive; magnitude is used).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav|Hop")
	static float HopVerticalVelocity(float ObstacleHeightUu, float GravityZ, float ClearanceMarginUu);

	// True when the AI should trigger a hop: grounded, high trace clear, and low trace hit within trigger distance.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav|Hop")
	static bool ShouldHop(float LowTraceHitDist, float TriggerDist, bool bHighTraceClear, bool bOnGround);
};
