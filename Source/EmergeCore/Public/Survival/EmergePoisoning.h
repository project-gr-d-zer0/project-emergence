#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergePoisoning.generated.h"

UCLASS()
class EMERGECORE_API UEmergePoisoning : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static float ToxicityDecayPerTick(float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static bool ShouldVomitFromToxicity(float Toxicity);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static bool PoisoningActive(int32 FoodPoisonAgentCount);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static bool VomitingActive(int32 FoodPoisonAgentCount);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static float InvertedAgentCount(int32 AgentCount, int32 AgentMax);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static float VomitChancePerTick(int32 AgentCount, int32 AgentMax, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static float VomitDuration(int32 AgentCount, int32 AgentMax);

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static int32 VomitWaterDrain();

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static int32 VomitEnergyDrain();

	UFUNCTION(BlueprintCallable, Category = "Survival|Poisoning")
	static int32 AgentReducePerVomit();
};