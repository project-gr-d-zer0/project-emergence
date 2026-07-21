// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: crosshair widen (shell/combat HUD).
// SHELL stratum: the dynamic crosshair gap - it opens up while moving and firing (reflecting real spread from
// EmergeSpreadStance) and recovers back to its resting size. Pure view math; deterministic.
// CONTRACT — NEW class UEmergeCrosshair : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeCrosshair.h + Private/Shell/EmergeCrosshair.cpp, static UFUNCTIONs.
//
//   static float SpreadGap(float BaseGap, float MoveSpeed01, bool bFiring);
//     // return BaseGap * (1.0f + MoveSpeed01 + (bFiring ? 0.5f : 0.0f));
//   static float RecoverGap(float Current, float BaseGap, float RecoverRate, float DeltaT);
//     // return FMath::Max(BaseGap, Current - RecoverRate * DeltaT);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeCrosshair.h"

BEGIN_DEFINE_SPEC(FEmergeCrosshairSpec, "Emergence.Shell.Crosshair",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCrosshairSpec)
void FEmergeCrosshairSpec::Define()
{
	Describe("SpreadGap", [this]()
	{
		It("opens with movement and firing", [this]()
		{
			TestEqual(TEXT("still, not firing -> base"), UEmergeCrosshair::SpreadGap(10.f, 0.f, false), 10.0f);
			TestEqual(TEXT("full move -> 2x"), UEmergeCrosshair::SpreadGap(10.f, 1.f, false), 20.0f);
			TestEqual(TEXT("still, firing -> 1.5x"), UEmergeCrosshair::SpreadGap(10.f, 0.f, true), 15.0f);
			TestEqual(TEXT("full move + firing -> 2.5x"), UEmergeCrosshair::SpreadGap(10.f, 1.f, true), 25.0f);
		});
	});

	Describe("RecoverGap", [this]()
	{
		It("recovers toward base, not below it", [this]()
		{
			TestEqual(TEXT("20 by 5 -> 15"), UEmergeCrosshair::RecoverGap(20.f, 10.f, 5.f, 1.0f), 15.0f);
			TestEqual(TEXT("12 by 5 clamps to base 10"), UEmergeCrosshair::RecoverGap(12.f, 10.f, 5.f, 1.0f), 10.0f);
			TestEqual(TEXT("already at base"), UEmergeCrosshair::RecoverGap(10.f, 10.f, 5.f, 1.0f), 10.0f);
		});
	});
}
#endif
