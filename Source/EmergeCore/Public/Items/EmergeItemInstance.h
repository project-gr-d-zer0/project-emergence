#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EmergeItemInstance.generated.h"

UCLASS()
class EMERGECORE_API UEmergeItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	float CurrentDurability;

	bool IsBroken(float currentDurability) const;
};