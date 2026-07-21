// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: map legend view-model (shell).
// SHELL stratum: maps a marker type to its icon, hostility flag, and draw priority (hostiles/objectives on top).
// Pairs with EmergeMinimapBlip. Pure view config; deterministic.
// CONTRACT — NEW class UEmergeMapLegend : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeMapLegend.h + Private/Shell/EmergeMapLegend.cpp, static UFUNCTIONs.
//   MarkerType int32: 0 Player, 1 Objective, 2 Loot, 3 Enemy, 4 Vehicle.
//
//   static int32 IconForType(int32 MarkerType);
//     // return (MarkerType >= 0 && MarkerType <= 4) ? MarkerType : 0;
//   static bool  IsHostileMarker(int32 MarkerType);   // return MarkerType == 3;
//   static float IconPriority(int32 MarkerType);
//     // switch: 3 -> 3.0f; 1 -> 2.0f; default -> 1.0f;   // enemies draw on top, then objectives
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeMapLegend.h"

BEGIN_DEFINE_SPEC(FEmergeMapLegendSpec, "Emergence.Shell.MapLegend",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMapLegendSpec)
void FEmergeMapLegendSpec::Define()
{
	Describe("Marker mapping", [this]()
	{
		It("icon, hostility, and draw priority", [this]()
		{
			TestEqual(TEXT("player icon 0"), UEmergeMapLegend::IconForType(0), 0);
			TestEqual(TEXT("enemy icon 3"), UEmergeMapLegend::IconForType(3), 3);
			TestEqual(TEXT("unknown -> 0"), UEmergeMapLegend::IconForType(9), 0);
			TestTrue(TEXT("enemy hostile"), UEmergeMapLegend::IsHostileMarker(3));
			TestFalse(TEXT("objective not hostile"), UEmergeMapLegend::IsHostileMarker(1));
			TestEqual(TEXT("enemy top priority"), UEmergeMapLegend::IconPriority(3), 3.0f);
			TestEqual(TEXT("objective priority 2"), UEmergeMapLegend::IconPriority(1), 2.0f);
			TestEqual(TEXT("loot priority 1"), UEmergeMapLegend::IconPriority(2), 1.0f);
		});
	});
}
#endif
