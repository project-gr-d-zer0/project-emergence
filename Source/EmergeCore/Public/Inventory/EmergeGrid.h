#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeGrid.generated.h"

// Deterministic grid/container packing math (modernized DayZ/Tarkov footprint fitting).
UCLASS()
class EMERGECORE_API UEmergeGrid : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Returns true when an ItemW x ItemH footprint placed at (X,Y) fits within a GridW x GridH container.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	static bool Fits(int32 GridW, int32 GridH, int32 X, int32 Y, int32 ItemW, int32 ItemH);

	// Returns the number of cells occupied by an ItemW x ItemH footprint, floored at 0.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	static int32 CellsUsed(int32 ItemW, int32 ItemH);
};
