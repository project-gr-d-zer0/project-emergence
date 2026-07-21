#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeStatusStrip.generated.h"

UCLASS()
class EMERGECORE_API UEmergeStatusStrip : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|StatusStrip")
	static float TimeRemaining01(float Elapsed, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Shell|StatusStrip")
	static bool IsExpired(float Elapsed, float Duration);

	UFUNCTION(BlueprintCallable, Category = "Shell|StatusStrip")
	static bool ShouldBlink(float Elapsed, float Duration, float BlinkThreshold01);
};