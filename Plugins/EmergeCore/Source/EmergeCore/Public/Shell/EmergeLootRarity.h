#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeLootRarity.generated.h"

UCLASS()
class EMERGECORE_API UEmergeLootRarity : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|LootRarity")
	static int32 RarityTier(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Shell|LootRarity")
	static float BorderGlow(int32 Tier);
};