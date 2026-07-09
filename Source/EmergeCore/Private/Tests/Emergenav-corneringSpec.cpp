// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: cornering speed scale.
// Pursuers slow while turning hard (Dying Light Volatile pattern) — this is the player's escape
// margin: straight-line chase speed stays honest, juking/corners open the gap.
// CONTRACT — CREATE ONE NEW class UEmergeCornering (UBlueprintFunctionLibrary, EMERGECORE_API) in
//   NEW files Public/Nav/EmergeCornering.h + Private/Nav/EmergeCornering.cpp:
//   static float SpeedScale(float TurnErrorDeg, float StartDeg, float FullDeg, float MinScale);
//     // const float Err = FMath::Abs(TurnErrorDeg);
//     // const float Min = FMath::Clamp(MinScale, 0.0f, 1.0f);
//     // if (FullDeg <= StartDeg) { return (Err >= FullDeg) ? Min : 1.0f; }
//     // const float T = FMath::Clamp((Err - StartDeg) / (FullDeg - StartDeg), 0.0f, 1.0f);
//     // return FMath::Lerp(1.0f, Min, T);
// Deterministic, exact golden values, boundaries covered. Every function body wrapped in braces;
// every code path must return a value.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeCornering.h"

BEGIN_DEFINE_SPEC(FEmergeCorneringSpec, "Emergence.Nav.Cornering",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCorneringSpec)
void FEmergeCorneringSpec::Define()
{
	Describe("SpeedScale (1.0 straight -> MinScale at hard turn)", [this]()
	{
		It("matches exact golden values at (start 20, full 60, min 0.65)", [this]()
		{
			TestEqual(TEXT("straight 0 -> 1"), UEmergeCornering::SpeedScale(0.f, 20.f, 60.f, 0.65f), 1.f);
			TestEqual(TEXT("at start 20 -> 1"), UEmergeCornering::SpeedScale(20.f, 20.f, 60.f, 0.65f), 1.f);
			TestEqual(TEXT("midway 40 -> 0.825"), UEmergeCornering::SpeedScale(40.f, 20.f, 60.f, 0.65f), 0.825f);
			TestEqual(TEXT("full 60 -> 0.65"), UEmergeCornering::SpeedScale(60.f, 20.f, 60.f, 0.65f), 0.65f);
			TestEqual(TEXT("beyond 90 -> 0.65"), UEmergeCornering::SpeedScale(90.f, 20.f, 60.f, 0.65f), 0.65f);
		});
		It("uses absolute turn error", [this]()
		{
			TestEqual(TEXT("-40 same as 40"), UEmergeCornering::SpeedScale(-40.f, 20.f, 60.f, 0.65f), 0.825f);
		});
		It("handles degenerate and clamped inputs", [this]()
		{
			TestEqual(TEXT("full<=start, below -> 1"), UEmergeCornering::SpeedScale(30.f, 40.f, 40.f, 0.5f), 1.f);
			TestEqual(TEXT("full<=start, at/above -> min"), UEmergeCornering::SpeedScale(50.f, 40.f, 40.f, 0.5f), 0.5f);
			TestEqual(TEXT("min clamped up from -1 -> 0"), UEmergeCornering::SpeedScale(90.f, 20.f, 60.f, -1.f), 0.f);
			TestEqual(TEXT("min clamped down from 2 -> 1"), UEmergeCornering::SpeedScale(90.f, 20.f, 60.f, 2.f), 1.f);
		});
	});
}
#endif

