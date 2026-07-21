#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWoundInfection.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWoundInfection : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|WoundInfection")
	static int32 InfectionStage(int32 WoundAgentCount);

	UFUNCTION(BlueprintCallable, Category = "Survival|WoundInfection")
	static bool HasFever(int32 WoundAgentCount);

	UFUNCTION(BlueprintCallable, Category = "Survival|WoundInfection")
	static float StageTwoDamagePerTick(float DeltaT);
};