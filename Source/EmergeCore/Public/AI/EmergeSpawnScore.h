#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSpawnScore.generated.h"

// Director spawn-candidate scoring (research: L4D 75-percent-behind rule + influence layers).
// Score = Threat + 0.75*BehindBias - 1.25*Presence. Higher score = better spawn candidate.
UCLASS()
class EMERGECORE_API UEmergeSpawnScore : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Converts a heading dot product into a 0..1 "behind the player" bias (1 = directly behind).
	UFUNCTION(BlueprintCallable, Category = "AI|Director")
	static float BehindBias01(float DotWithHeading);

	// Weighted composite spawn score across the three influence layers.
	UFUNCTION(BlueprintCallable, Category = "AI|Director")
	static float Score(float Threat, float BehindBias, float Presence);

	// True if the candidate is within the camera's view frustum (caller should reject visible spawns).
	UFUNCTION(BlueprintCallable, Category = "AI|Director")
	static bool IsInViewFrustum(float DotCamToCandidate, float CosHalfFovPlusMargin);
};
