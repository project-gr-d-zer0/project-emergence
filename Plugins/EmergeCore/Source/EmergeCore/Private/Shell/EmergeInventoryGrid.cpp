#include "Shell/EmergeInventoryGrid.h"

int32 UEmergeInventoryGrid::CellCount(int32 ItemW, int32 ItemH)
{
	return ItemW * ItemH;
}

bool UEmergeInventoryGrid::FitsAt(int32 Col, int32 Row, int32 ItemW, int32 ItemH, int32 GridCols, int32 GridRows)
{
	return Col >= 0 && Row >= 0 && (Col + ItemW) <= GridCols && (Row + ItemH) <= GridRows;
}

int32 UEmergeInventoryGrid::FreeCells(int32 GridCols, int32 GridRows, int32 UsedCells)
{
	return (GridCols * GridRows) - UsedCells;
}

bool UEmergeInventoryGrid::HasSpaceFor(int32 ItemW, int32 ItemH, int32 GridCols, int32 GridRows, int32 UsedCells)
{
	return CellCount(ItemW, ItemH) <= FreeCells(GridCols, GridRows, UsedCells);
}