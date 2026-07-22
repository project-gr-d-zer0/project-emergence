#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergePing.generated.h"

UCLASS()
class EMERGECORE_API UEmergePing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Ping")
	static float FadeAlpha(float Elapsed, float Lifetime);

	UFUNCTION(BlueprintCallable, Category = "Shell|Ping")
	static bool IsExpired(float Elapsed, float Lifetime);

	UFUNCTION(BlueprintCallable, Category = "Shell|Ping")
	static float DistanceScale(float Distance, float RefDistance);
};