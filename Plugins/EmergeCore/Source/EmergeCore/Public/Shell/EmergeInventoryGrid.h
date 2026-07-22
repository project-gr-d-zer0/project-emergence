#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeInventoryGrid.generated.h"

// Deterministic slot-grid placement math for the inventory view-model (Tarkov/DayZ-style).
UCLASS()
class EMERGECORE_API UEmergeInventoryGrid : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	static int32 CellCount(int32 ItemW, int32 ItemH);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	static bool FitsAt(int32 Col, int32 Row, int32 ItemW, int32 ItemH, int32 GridCols, int32 GridRows);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	static int32 FreeCells(int32 GridCols, int32 GridRows, int32 UsedCells);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	static bool HasSpaceFor(int32 ItemW, int32 ItemH, int32 GridCols, int32 GridRows, int32 UsedCells);
};