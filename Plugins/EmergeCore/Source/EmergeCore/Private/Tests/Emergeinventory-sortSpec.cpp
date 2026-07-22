// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: inventory sort keys (shell).
// SHELL stratum: the comparison primitives behind inventory sorting - by weight (ascending) or value
// (descending) - and a generic swap decision for a sort pass. Pure comparison logic; deterministic.
// CONTRACT — NEW class UEmergeInventorySort : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeInventorySort.h + Private/Shell/EmergeInventorySort.cpp, static UFUNCTIONs.
//
//   static int32 CompareByWeight(float WeightA, float WeightB);
//     // if (WeightA < WeightB) return -1;  if (WeightA > WeightB) return 1;  return 0;
//   static int32 CompareByValue(int32 ValueA, int32 ValueB);   // descending (higher value first):
//     // if (ValueA > ValueB) return -1;  if (ValueA < ValueB) return 1;  return 0;
//   static bool  ShouldSwap(float KeyA, float KeyB, bool bAscending);
//     // return bAscending ? (KeyA > KeyB) : (KeyA < KeyB);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeInventorySort.h"

BEGIN_DEFINE_SPEC(FEmergeInventorySortSpec, "Emergence.Shell.InventorySort",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInventorySortSpec)
void FEmergeInventorySortSpec::Define()
{
	Describe("Comparators", [this]()
	{
		It("weight ascending, value descending", [this]()
		{
			TestEqual(TEXT("1<2 weight -> -1"), UEmergeInventorySort::CompareByWeight(1.f, 2.f), -1);
			TestEqual(TEXT("2>1 weight -> 1"), UEmergeInventorySort::CompareByWeight(2.f, 1.f), 1);
			TestEqual(TEXT("equal weight -> 0"), UEmergeInventorySort::CompareByWeight(1.f, 1.f), 0);
			TestEqual(TEXT("100>50 value -> -1 (desc)"), UEmergeInventorySort::CompareByValue(100, 50), -1);
			TestEqual(TEXT("50<100 value -> 1"), UEmergeInventorySort::CompareByValue(50, 100), 1);
			TestEqual(TEXT("equal value -> 0"), UEmergeInventorySort::CompareByValue(50, 50), 0);
		});
	});

	Describe("ShouldSwap", [this]()
	{
		It("swap decision respects direction", [this]()
		{
			TestTrue(TEXT("asc: 2 before 1 -> swap"), UEmergeInventorySort::ShouldSwap(2.f, 1.f, true));
			TestFalse(TEXT("asc: 1 before 2 -> keep"), UEmergeInventorySort::ShouldSwap(1.f, 2.f, true));
			TestTrue(TEXT("desc: 1 before 2 -> swap"), UEmergeInventorySort::ShouldSwap(1.f, 2.f, false));
		});
	});
}
#endif
