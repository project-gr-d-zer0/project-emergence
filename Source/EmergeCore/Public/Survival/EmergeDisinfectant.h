#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeDisinfectant.generated.h"

UCLASS()
class EMERGECORE_API UEmergeDisinfectant : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Disinfectant")
	static bool CanCure(int32 InfectionStage);

	UFUNCTION(BlueprintCallable, Category = "Survival|Disinfectant")
	static bool RequiresAntibiotics(int32 InfectionStage);

	UFUNCTION(BlueprintCallable, Category = "Survival|Disinfectant")
	static float AgentReductionPerTick(float DeltaT);
};