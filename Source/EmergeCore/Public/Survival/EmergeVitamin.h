#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeVitamin.generated.h"

UCLASS()
class EMERGECORE_API UEmergeVitamin : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Vitamin")
	static float LifetimeSecs();

	UFUNCTION(BlueprintCallable, Category = "Survival|Vitamin")
	static float RemainingSecs(float Elapsed);

	UFUNCTION(BlueprintCallable, Category = "Survival|Vitamin")
	static bool IsActive(float Elapsed);

	UFUNCTION(BlueprintCallable, Category = "Survival|Vitamin")
	static int32 BoostedImmunityLevel();
};