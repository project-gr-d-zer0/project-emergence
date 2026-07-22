#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHotbar.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHotbar : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Wraps an index into the valid range [0, SlotCount)
     * @param Index The input index (can be negative or exceed SlotCount)
     * @param SlotCount The number of slots in the hotbar
     * @return The wrapped index in [0, SlotCount)
     */
    UFUNCTION(BlueprintPure, Category = "Emergence|Shell")
    static int32 WrapSlot(int32 Index, int32 SlotCount);

    /**
     * Gets the next slot index, wrapping around if necessary
     * @param Current The current slot index
     * @param SlotCount The number of slots in the hotbar
     * @return The next slot index
     */
    UFUNCTION(BlueprintPure, Category = "Emergence|Shell")
    static int32 NextSlot(int32 Current, int32 SlotCount);

    /**
     * Gets the previous slot index, wrapping around if necessary
     * @param Current The current slot index
     * @param SlotCount The number of slots in the hotbar
     * @return The previous slot index
     */
    UFUNCTION(BlueprintPure, Category = "Emergence|Shell")
    static int32 PrevSlot(int32 Current, int32 SlotCount);
};