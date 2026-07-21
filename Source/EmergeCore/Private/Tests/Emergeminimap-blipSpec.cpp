// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: minimap blip projection (shell).
// SHELL stratum: projects a world coordinate onto normalized [0,1] minimap space (per axis) and tests whether a
// point is within the mapped world bounds (to cull off-map blips). Pure projection math; deterministic.
// CONTRACT — NEW class UEmergeMinimapBlip : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeMinimapBlip.h + Private/Shell/EmergeMinimapBlip.cpp, static UFUNCTIONs.
//
//   static float WorldToMap01(float WorldCoord, float WorldMin, float WorldMax);
//     // return FMath::Clamp((WorldCoord - WorldMin) / (WorldMax - WorldMin), 0.0f, 1.0f);
//   static bool  IsOnMap(float WorldCoord, float WorldMin, float WorldMax);
//     // return WorldCoord >= WorldMin && WorldCoord <= WorldMax;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeMinimapBlip.h"

BEGIN_DEFINE_SPEC(FEmergeMinimapBlipSpec, "Emergence.Shell.MinimapBlip",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMinimapBlipSpec)
void FEmergeMinimapBlipSpec::Define()
{
	Describe("WorldToMap01 & IsOnMap", [this]()
	{
		It("projects world coords to [0,1] and culls off-map", [this]()
		{
			TestEqual(TEXT("center -> 0.5"), UEmergeMinimapBlip::WorldToMap01(500.f, 0.f, 1000.f), 0.5f);
			TestEqual(TEXT("min edge -> 0"), UEmergeMinimapBlip::WorldToMap01(0.f, 0.f, 1000.f), 0.0f);
			TestEqual(TEXT("max edge -> 1"), UEmergeMinimapBlip::WorldToMap01(1000.f, 0.f, 1000.f), 1.0f);
			TestEqual(TEXT("beyond -> clamped 1"), UEmergeMinimapBlip::WorldToMap01(1500.f, 0.f, 1000.f), 1.0f);
			TestTrue(TEXT("inside is on map"), UEmergeMinimapBlip::IsOnMap(500.f, 0.f, 1000.f));
			TestFalse(TEXT("outside culled"), UEmergeMinimapBlip::IsOnMap(1500.f, 0.f, 1000.f));
		});
	});
}
#endif
