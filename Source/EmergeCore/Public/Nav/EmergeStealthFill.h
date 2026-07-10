#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStealthFill.generated.h"

// Stealth detection multiplier stack (research: Blacklist gates + TLoU fill —
// stance x light x motion; light is floored, NEVER a gate).
UCLASS()
class EMERGECORE_API UEmergeStealthFill : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Stealth")
	static float StanceMultiplier(int32 Stance);

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	static float LightMultiplier(float Light01);

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	static float MotionMultiplier(float Speed01);

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	static float FillMultiplier(int32 Stance, float Light01, float Speed01);

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	static bool PassesVisibilityGate(float VisibleFraction);
};
