// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hotbar / quickslot selection (shell).
// SHELL stratum: cycling the active quickslot with wrap-around (scroll wheel / number keys), including negative
// indices from scrolling up past slot 0. Pure index math; deterministic.
// CONTRACT — NEW class UEmergeHotbar : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeHotbar.h + Private/Shell/EmergeHotbar.cpp, static UFUNCTIONs.
//
//   static int32 WrapSlot(int32 Index, int32 SlotCount);   // return ((Index % SlotCount) + SlotCount) % SlotCount;
//   static int32 NextSlot(int32 Current, int32 SlotCount);  // return WrapSlot(Current + 1, SlotCount);
//   static int32 PrevSlot(int32 Current, int32 SlotCount);  // return WrapSlot(Current - 1, SlotCount);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeHotbar.h"

BEGIN_DEFINE_SPEC(FEmergeHotbarSpec, "Emergence.Shell.Hotbar",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHotbarSpec)
void FEmergeHotbarSpec::Define()
{
	Describe("WrapSlot", [this]()
	{
		It("wraps positive and negative indices into [0,SlotCount)", [this]()
		{
			TestEqual(TEXT("0 stays"), UEmergeHotbar::WrapSlot(0, 5), 0);
			TestEqual(TEXT("5 wraps to 0"), UEmergeHotbar::WrapSlot(5, 5), 0);
			TestEqual(TEXT("7 wraps to 2"), UEmergeHotbar::WrapSlot(7, 5), 2);
			TestEqual(TEXT("-1 wraps to 4"), UEmergeHotbar::WrapSlot(-1, 5), 4);
			TestEqual(TEXT("-6 wraps to 4"), UEmergeHotbar::WrapSlot(-6, 5), 4);
		});
	});

	Describe("NextSlot / PrevSlot", [this]()
	{
		It("scrolls with wrap-around", [this]()
		{
			TestEqual(TEXT("next 4 -> 0"), UEmergeHotbar::NextSlot(4, 5), 0);
			TestEqual(TEXT("next 0 -> 1"), UEmergeHotbar::NextSlot(0, 5), 1);
			TestEqual(TEXT("prev 0 -> 4"), UEmergeHotbar::PrevSlot(0, 5), 4);
			TestEqual(TEXT("prev 3 -> 2"), UEmergeHotbar::PrevSlot(3, 5), 2);
		});
	});
}
#endif
