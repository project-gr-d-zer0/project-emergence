#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeBandage.generated.h"

UCLASS()
class EMERGECORE_API UEmergeBandage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Bandage")
	static int32 SourcesClosedPerApplication();

	UFUNCTION(BlueprintCallable, Category = "Survival|Bandage")
	static int32 RemainingSources(int32 SourcesBefore);

	UFUNCTION(BlueprintCallable, Category = "Survival|Bandage")
	static bool StopsAllBleeding(int32 SourcesBefore);

	UFUNCTION(BlueprintCallable, Category = "Survival|Bandage")
	static float BandageTimeSec();

	UFUNCTION(BlueprintCallable, Category = "Survival|Bandage")
	static float DressingInfectionChance(bool bClean);

	UFUNCTION(BlueprintCallable, Category = "Survival|Bandage")
	static int32 QuantityConsumed();
};