// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: radial quickslot menu (shell).
// SHELL stratum: the radial (wheel) menu selection - maps the cursor angle to the nearest wedge/slot and reports
// each slot's center angle for layout. Pure angular math; deterministic.
// CONTRACT — NEW class UEmergeRadialMenu : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeRadialMenu.h + Private/Shell/EmergeRadialMenu.cpp, static UFUNCTIONs.
//
//   static int32 SlotFromAngle(float AngleDeg, int32 SlotCount);
//     // return ((int32)FMath::RoundToInt(AngleDeg / (360.0f / SlotCount))) % SlotCount;
//   static float SlotCenterAngle(int32 Slot, int32 SlotCount);
//     // return Slot * (360.0f / SlotCount);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeRadialMenu.h"

BEGIN_DEFINE_SPEC(FEmergeRadialMenuSpec, "Emergence.Shell.RadialMenu",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeRadialMenuSpec)
void FEmergeRadialMenuSpec::Define()
{
	Describe("SlotFromAngle (8-slot wheel)", [this]()
	{
		It("maps cursor angle to nearest wedge, wrapping", [this]()
		{
			TestEqual(TEXT("0 -> slot 0"), UEmergeRadialMenu::SlotFromAngle(0.f, 8), 0);
			TestEqual(TEXT("45 -> slot 1"), UEmergeRadialMenu::SlotFromAngle(45.f, 8), 1);
			TestEqual(TEXT("90 -> slot 2"), UEmergeRadialMenu::SlotFromAngle(90.f, 8), 2);
			TestEqual(TEXT("350 wraps to 0"), UEmergeRadialMenu::SlotFromAngle(350.f, 8), 0);
			TestEqual(TEXT("23 rounds to slot 1"), UEmergeRadialMenu::SlotFromAngle(23.f, 8), 1);
			TestEqual(TEXT("22 rounds to slot 0"), UEmergeRadialMenu::SlotFromAngle(22.f, 8), 0);
		});
	});

	Describe("SlotCenterAngle", [this]()
	{
		It("evenly spaced wedge centers", [this]()
		{
			TestEqual(TEXT("slot 0 -> 0deg"), UEmergeRadialMenu::SlotCenterAngle(0, 8), 0.0f);
			TestEqual(TEXT("slot 1 -> 45deg"), UEmergeRadialMenu::SlotCenterAngle(1, 8), 45.0f);
			TestEqual(TEXT("slot 2 -> 90deg"), UEmergeRadialMenu::SlotCenterAngle(2, 8), 90.0f);
			TestEqual(TEXT("slot 4 of 4 -> 90deg"), UEmergeRadialMenu::SlotCenterAngle(1, 4), 90.0f);
		});
	});
}
#endif
