// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: grid/container packing (modernized DayZ/Tarkov).
// Items have a WxH footprint and must fit within a WxH container grid at a given cell — the "modernized DayZ
// inventory" differentiator. Deterministic placement math, testable headless.
// CONTRACT — NEW class UEmergeGrid : public UBlueprintFunctionLibrary
//   in Public/Inventory/EmergeGrid.h + Private/Inventory/EmergeGrid.cpp, static UFUNCTIONs:
//   static bool  Fits(int32 GridW, int32 GridH, int32 X, int32 Y, int32 ItemW, int32 ItemH);
//     // return X >= 0 && Y >= 0 && ItemW > 0 && ItemH > 0 && (X + ItemW) <= GridW && (Y + ItemH) <= GridH;
//   static int32 CellsUsed(int32 ItemW, int32 ItemH);   // return FMath::Max(0,ItemW) * FMath::Max(0,ItemH);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeGrid.h"

BEGIN_DEFINE_SPEC(FEmergeGridSpec, "Emergence.Inventory.Grid",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGridSpec)
void FEmergeGridSpec::Define()
{
	Describe("Fits (item footprint within a 10x6 container)", [this]()
	{
		It("accepts in-bounds placements and rejects overflow/negative/degenerate", [this]()
		{
			TestTrue(TEXT("2x3 at (0,0) fits"),           UEmergeGrid::Fits(10, 6, 0, 0, 2, 3));
			TestTrue(TEXT("2x3 at (8,3) fits exactly"),   UEmergeGrid::Fits(10, 6, 8, 3, 2, 3));
			TestFalse(TEXT("2x3 at (9,0) overflows X"),   UEmergeGrid::Fits(10, 6, 9, 0, 2, 3));
			TestFalse(TEXT("2x3 at (0,4) overflows Y"),   UEmergeGrid::Fits(10, 6, 0, 4, 2, 3));
			TestFalse(TEXT("negative X rejected"),        UEmergeGrid::Fits(10, 6, -1, 0, 2, 3));
			TestFalse(TEXT("zero-width item rejected"),   UEmergeGrid::Fits(10, 6, 0, 0, 0, 3));
		});
	});

	Describe("CellsUsed (footprint area)", [this]()
	{
		It("matches exact golden values and floors at 0", [this]()
		{
			TestEqual(TEXT("2x3 -> 6"),        UEmergeGrid::CellsUsed(2, 3), 6);
			TestEqual(TEXT("1x1 -> 1"),        UEmergeGrid::CellsUsed(1, 1), 1);
			TestEqual(TEXT("0x5 -> 0"),        UEmergeGrid::CellsUsed(0, 5), 0);
			TestEqual(TEXT("negative -> 0"),   UEmergeGrid::CellsUsed(-2, 3), 0);
		});
	});
}
#endif
