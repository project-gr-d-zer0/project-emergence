// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: flee-candidate scoring.
// The dynamic-escape picker (AEmergeCharacter::FleeFrom) scores each reachable flee candidate:
// escape distance actually gained minus a detour penalty from the real nav path length. This kernel
// pins that exact formula so the structural code and tests can never drift apart.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeFlee (do NOT rename/duplicate;
//   keep FleeDirection/FleePoint/ThreatLevel exactly), in the existing
//   Public/Nav/EmergeFlee.h + Private/Nav/EmergeFlee.cpp:
//   static float ScoreFleeCandidate(float EscapeGain, float PathLength, float StraightLineDist, float DetourPenalty);
//     // const float Straight = FMath::Max(StraightLineDist, 1.0f);
//     // const float Detour = PathLength / Straight;
//     // return EscapeGain - (Detour - 1.0f) * DetourPenalty;
// Deterministic, exact golden values, boundaries covered. Every function body wrapped in braces;
// every code path must return a value.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeFlee.h"

BEGIN_DEFINE_SPEC(FEmergeFleeScoreSpec, "Emergence.Nav.FleeScore",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFleeScoreSpec)
void FEmergeFleeScoreSpec::Define()
{
	Describe("ScoreFleeCandidate (escape gain minus detour penalty)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("straight path, pure gain: 4000 gain, no detour -> 4000"),
				UEmergeFlee::ScoreFleeCandidate(4000.f, 6000.f, 6000.f, 1500.f), 4000.f);
			TestEqual(TEXT("1.5x detour costs 750: 4000 - 0.5*1500 -> 3250"),
				UEmergeFlee::ScoreFleeCandidate(4000.f, 9000.f, 6000.f, 1500.f), 3250.f);
			TestEqual(TEXT("2x detour costs full penalty: 3000 - 1500 -> 1500"),
				UEmergeFlee::ScoreFleeCandidate(3000.f, 12000.f, 6000.f, 1500.f), 1500.f);
			TestEqual(TEXT("negative gain (routes toward threat) scores negative"),
				UEmergeFlee::ScoreFleeCandidate(-500.f, 6000.f, 6000.f, 1500.f), -500.f);
		});
		It("clamps the straight-line floor to 1", [this]()
		{
			TestEqual(TEXT("zero straight dist treated as 1: 100 - (50-1)*10 -> -390"),
				UEmergeFlee::ScoreFleeCandidate(100.f, 50.f, 0.f, 10.f), -390.f);
		});
	});
}
#endif

