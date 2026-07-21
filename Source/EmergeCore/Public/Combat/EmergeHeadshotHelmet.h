#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHeadshotHelmet.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHeadshotHelmet : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Headshot")
	static float HeadshotMultiplier(float BaseHeadMult, int32 HelmetClass);

	UFUNCTION(BlueprintCallable, Category = "Combat|Headshot")
	static bool StopsHeadshotKill(int32 HelmetClass, float PenPower);
};