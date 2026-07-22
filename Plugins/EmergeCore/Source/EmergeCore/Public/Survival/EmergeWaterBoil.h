#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWaterBoil.generated.h"

UCLASS()
class EMERGECORE_API UEmergeWaterBoil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|WaterBoil")
	static float BoilProgressPerTick(bool bOnHeat, float DeltaT);

	UFUNCTION(BlueprintCallable, Category = "Survival|WaterBoil")
	static bool IsPurified(float BoilProgress, float BoilTimeReq);

	UFUNCTION(BlueprintCallable, Category = "Survival|WaterBoil")
	static float ContaminationAfterBoil(float BaseChance, bool bPurified);
};