// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: director spawn-candidate scoring
// (research: L4D 75-percent-behind + influence layers — Threat + 0.75*BehindBias - 1.25*Presence).
// CONTRACT — NEW class UEmergeSpawnScore : public UBlueprintFunctionLibrary
//   in Public/AI/EmergeSpawnScore.h + Private/AI/EmergeSpawnScore.cpp, static UFUNCTIONs:
//   static float BehindBias01(float DotWithHeading);   // = FMath::Clamp(-DotWithHeading, 0.f, 1.f)
//   static float Score(float Threat, float BehindBias, float Presence);
//     // = Threat + 0.75f * BehindBias - 1.25f * Presence
//   static bool IsInViewFrustum(float DotCamToCandidate, float CosHalfFovPlusMargin);
//     // = DotCamToCandidate > CosHalfFovPlusMargin   (true = visible; caller rejects)
// Deterministic, exact golden values.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "AI/EmergeSpawnScore.h"

BEGIN_DEFINE_SPEC(FEmergeSpawnScoreSpec, "Emergence.AI.SpawnScore",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSpawnScoreSpec)
void FEmergeSpawnScoreSpec::Define()
{
	Describe("BehindBias01", [this]()
	{
		It("is 1 behind, 0 ahead", [this]()
		{
			TestEqual(TEXT("dot -1 -> 1"), UEmergeSpawnScore::BehindBias01(-1.f), 1.f);
			TestEqual(TEXT("dot -0.5 -> 0.5"), UEmergeSpawnScore::BehindBias01(-0.5f), 0.5f);
			TestEqual(TEXT("dot 0 -> 0"), UEmergeSpawnScore::BehindBias01(0.f), 0.f);
			TestEqual(TEXT("dot 0.7 -> 0"), UEmergeSpawnScore::BehindBias01(0.7f), 0.f);
		});
	});
	Describe("Score and frustum", [this]()
	{
		It("weights the three layers", [this]()
		{
			TestNearlyEqual(TEXT("T1 B1 P0 -> 1.75"), UEmergeSpawnScore::Score(1.f, 1.f, 0.f), 1.75f, 1e-4f);
			TestNearlyEqual(TEXT("T0.5 B0 P0.4 -> 0"), UEmergeSpawnScore::Score(0.5f, 0.f, 0.4f), 0.f, 1e-4f);
			TestNearlyEqual(TEXT("T0 B0.8 P1 -> -0.65"), UEmergeSpawnScore::Score(0.f, 0.8f, 1.f), -0.65f, 1e-4f);
		});
		It("frustum check is strict greater-than", [this]()
		{
			TestTrue(TEXT("0.9 > 0.5 visible"), UEmergeSpawnScore::IsInViewFrustum(0.9f, 0.5f));
			TestFalse(TEXT("0.4 hidden"), UEmergeSpawnScore::IsInViewFrustum(0.4f, 0.5f));
			TestFalse(TEXT("0.5 boundary hidden"), UEmergeSpawnScore::IsInViewFrustum(0.5f, 0.5f));
		});
	});
}
#endif
