// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: play-rate + stride-scale split solver
// (research: Lyra-style foot-slide fix — rate clamped 1.0..1.2, stride covers the remainder, clamp 0.9..1.35).
// CONTRACT — NEW class UEmergeStrideSolve : public UBlueprintFunctionLibrary
//   in Public/Anim/EmergeStrideSolve.h + Private/Anim/EmergeStrideSolve.cpp, static UFUNCTIONs:
//   static float PlayRate(float SpeedUu, float AuthoredSpeedUu);
//     // = FMath::Clamp(SpeedUu / FMath::Max(AuthoredSpeedUu, 1.f), 1.f, 1.2f)
//   static float StrideScale(float SpeedUu, float AuthoredSpeedUu, float InPlayRate);
//     // = FMath::Clamp(SpeedUu / (FMath::Max(AuthoredSpeedUu, 1.f) * FMath::Max(InPlayRate, 0.01f)), 0.9f, 1.35f)
// Deterministic; float comparisons use tolerance.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Anim/EmergeStrideSolve.h"

BEGIN_DEFINE_SPEC(FEmergeStrideSolveSpec, "Emergence.Anim.StrideSolve",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStrideSolveSpec)
void FEmergeStrideSolveSpec::Define()
{
	Describe("PlayRate", [this]()
	{
		It("clamps to 1.0..1.2", [this]()
		{
			TestNearlyEqual(TEXT("560/375 -> 1.2 (clamped)"), UEmergeStrideSolve::PlayRate(560.f, 375.f), 1.2f, 1e-4f);
			TestNearlyEqual(TEXT("400/375 -> 1.0667"), UEmergeStrideSolve::PlayRate(400.f, 375.f), 400.f/375.f, 1e-4f);
			TestNearlyEqual(TEXT("slower -> 1.0 floor"), UEmergeStrideSolve::PlayRate(300.f, 375.f), 1.f, 1e-4f);
		});
	});
	Describe("StrideScale", [this]()
	{
		It("covers the residual after play rate", [this]()
		{
			// 560 / (375 * 1.2) = 1.2444
			TestNearlyEqual(TEXT("zombie chase case"), UEmergeStrideSolve::StrideScale(560.f, 375.f, 1.2f), 560.f/450.f, 1e-4f);
			TestNearlyEqual(TEXT("matched speed -> 1.0"), UEmergeStrideSolve::StrideScale(375.f, 375.f, 1.f), 1.f, 1e-4f);
			TestNearlyEqual(TEXT("upper clamp 1.35"), UEmergeStrideSolve::StrideScale(800.f, 375.f, 1.f), 1.35f, 1e-4f);
			TestNearlyEqual(TEXT("lower clamp 0.9"), UEmergeStrideSolve::StrideScale(200.f, 375.f, 1.f), 0.9f, 1e-4f);
		});
	});
}
#endif
