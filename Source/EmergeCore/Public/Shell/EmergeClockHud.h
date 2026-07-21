#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeClockHud.generated.h"

UCLASS()
class EMERGECORE_API UEmergeClockHud : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|ClockHud")
	static int32 HourOfDay(float SecondsOfDay);

	UFUNCTION(BlueprintCallable, Category = "Shell|ClockHud")
	static int32 MinuteOfHour(float SecondsOfDay);

	UFUNCTION(BlueprintCallable, Category = "Shell|ClockHud")
	static bool IsDaytime(int32 Hour);
};