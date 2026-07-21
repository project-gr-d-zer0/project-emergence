#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBloodTransfusion.generated.h"

UCLASS()
class EMERGECORE_API UEmergeBloodTransfusion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|BloodTransfusion")
	static bool IsCompatible(int32 DonorAbo, bool bDonorRhPos, int32 RecipAbo, bool bRecipRhPos);

	UFUNCTION(BlueprintCallable, Category = "Survival|BloodTransfusion")
	static int32 TransfusionReaction(bool bCompatible, float BloodAmount);

	UFUNCTION(BlueprintCallable, Category = "Survival|BloodTransfusion")
	static float HemolyticDrainPerTick(float DeltaT);
};