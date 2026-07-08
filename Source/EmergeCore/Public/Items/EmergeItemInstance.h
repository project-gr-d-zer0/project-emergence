#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EmergeItemInstance.generated.h"

// This class represents an instance of an item in the game.
UCLASS()
class EMERGECORE_API UEmergeItemInstance : public UObject
{
	GENERATED_BODY()

public:
	// Current durability of the item.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	float CurrentDurability;

	// New Method to Check if Item is Broken
	bool IsBroken(float currentDurability) const;
};