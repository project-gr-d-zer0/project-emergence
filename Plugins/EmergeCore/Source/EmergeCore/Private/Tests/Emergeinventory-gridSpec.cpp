// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: inventory slot-grid view-model (Tarkov/DayZ-style).
// SHELL stratum: the grid-inventory placement math. An item occupies a WxH block of cells; it fits at a position
// only if the block lies fully inside the grid; a simple capacity check gates whether it can go in at all. Pure
// grid geometry, no UMG. Deterministic.
// CONTRACT — NEW class UEmergeInventoryGrid : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeInventoryGrid.h + Private/Shell/EmergeInventoryGrid.cpp, static UFUNCTIONs.
//
//   static int32 CellCount(int32 ItemW, int32 ItemH);   // return ItemW * ItemH;
//   static bool  FitsAt(int32 Col, int32 Row, int32 ItemW, int32 ItemH, int32 GridCols, int32 GridRows);
//     // return Col >= 0 && Row >= 0 && (Col + ItemW) <= GridCols && (Row + ItemH) <= GridRows;
//   static int32 FreeCells(int32 GridCols, int32 GridRows, int32 UsedCells);
//     // return (GridCols * GridRows) - UsedCells;
//   static bool  HasSpaceFor(int32 ItemW, int32 ItemH, int32 GridCols, int32 GridRows, int32 UsedCells);
//     // return CellCount(ItemW, ItemH) <= FreeCells(GridCols, GridRows, UsedCells);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeInventoryGrid.h"

BEGIN_DEFINE_SPEC(FEmergeInventoryGridSpec, "Emergence.Shell.InventoryGrid",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInventoryGridSpec)
void FEmergeInventoryGridSpec::Define()
{
	Describe("CellCount & FreeCells", [this]()
	{
		It("cells = w*h; free = capacity - used", [this]()
		{
			TestEqual(TEXT("2x3 -> 6"), UEmergeInventoryGrid::CellCount(2, 3), 6);
			TestEqual(TEXT("1x1 -> 1"), UEmergeInventoryGrid::CellCount(1, 1), 1);
			TestEqual(TEXT("4x4 grid, 10 used -> 6 free"), UEmergeInventoryGrid::FreeCells(4, 4, 10), 6);
			TestEqual(TEXT("full grid -> 0 free"), UEmergeInventoryGrid::FreeCells(4, 4, 16), 0);
		});
	});

	Describe("FitsAt", [this]()
	{
		It("item block must lie fully inside the grid", [this]()
		{
			TestTrue(TEXT("2x2 at (0,0) in 4x4"), UEmergeInventoryGrid::FitsAt(0, 0, 2, 2, 4, 4));
			TestTrue(TEXT("2x2 at (2,2) in 4x4 (flush)"), UEmergeInventoryGrid::FitsAt(2, 2, 2, 2, 4, 4));
			TestFalse(TEXT("2x2 at (3,0) overflows cols"), UEmergeInventoryGrid::FitsAt(3, 0, 2, 2, 4, 4));
			TestFalse(TEXT("2x2 at (0,3) overflows rows"), UEmergeInventoryGrid::FitsAt(0, 3, 2, 2, 4, 4));
			TestFalse(TEXT("negative position"), UEmergeInventoryGrid::FitsAt(-1, 0, 1, 1, 4, 4));
		});
	});

	Describe("HasSpaceFor", [this]()
	{
		It("capacity check by free-cell count", [this]()
		{
			TestTrue(TEXT("2x2 into 4 free"), UEmergeInventoryGrid::HasSpaceFor(2, 2, 4, 4, 12));
			TestFalse(TEXT("2x2 into 3 free"), UEmergeInventoryGrid::HasSpaceFor(2, 2, 4, 4, 13));
			TestTrue(TEXT("1x1 into 1 free"), UEmergeInventoryGrid::HasSpaceFor(1, 1, 4, 4, 15));
		});
	});
}
#endif
