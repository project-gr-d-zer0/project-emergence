#include "Inventory/EmergeGrid.h"

bool UEmergeGrid::Fits(int32 GridW, int32 GridH, int32 X, int32 Y, int32 ItemW, int32 ItemH)
{
	return X >= 0 && Y >= 0 && ItemW > 0 && ItemH > 0 && (X + ItemW) <= GridW && (Y + ItemH) <= GridH;
}

int32 UEmergeGrid::CellsUsed(int32 ItemW, int32 ItemH)
{
	return FMath::Max(0, ItemW) * FMath::Max(0, ItemH);
}
