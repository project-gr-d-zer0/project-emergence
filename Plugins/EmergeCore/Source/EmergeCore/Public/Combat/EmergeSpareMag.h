#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSpareMag.generated.h"

UCLASS()
class EMERGECORE_API UEmergeSpareMag : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Ammo")
	static int32 TotalReserveRounds(int32 SpareMagCount, int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Combat|Ammo")
	static int32 TotalCarried(int32 LoadedRounds, int32 SpareMagCount, int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Combat|Ammo")
	static bool HasSpareMag(int32 SpareMagCount);
};