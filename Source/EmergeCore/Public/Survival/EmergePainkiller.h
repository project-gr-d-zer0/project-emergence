#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergePainkiller.generated.h"

UCLASS()
class EMERGECORE_API UEmergePainkiller : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Painkiller")
	static float PainReduction(int32 DrugType);

	UFUNCTION(BlueprintCallable, Category = "Survival|Painkiller")
	static float DurationSec(int32 DrugType);

	UFUNCTION(BlueprintCallable, Category = "Survival|Painkiller")
	static bool SuppressesFracturePain(int32 DrugType);
};