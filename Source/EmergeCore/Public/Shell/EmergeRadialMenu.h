#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeRadialMenu.generated.h"

UCLASS()
class EMERGECORE_API UEmergeRadialMenu : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Maps a cursor angle to the nearest radial menu slot
     * @param AngleDeg The cursor angle in degrees (0-360)
     * @param SlotCount The number of slots in the radial menu
     * @return The slot index (0 to SlotCount-1)
     */
    UFUNCTION(BlueprintPure, Category = "Emergence|Shell")
    static int32 SlotFromAngle(float AngleDeg, int32 SlotCount);

    /**
     * Gets the center angle for a given radial menu slot
     * @param Slot The slot index (0 to SlotCount-1)
     * @param SlotCount The number of slots in the radial menu
     * @return The center angle in degrees for the slot
     */
    UFUNCTION(BlueprintPure, Category = "Emergence|Shell")
    static float SlotCenterAngle(int32 Slot, int32 SlotCount);
};