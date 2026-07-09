// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ETA + progress telemetry for GetNavProgress.
// CONTRACT — NEW class UEmergeNavEta : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeNavEta.h + Private/Nav/EmergeNavEta.cpp, static UFUNCTIONs:
//   static float EtaSeconds(float RemainingLength, float SpeedUUPerSec);
//     // if (SpeedUUPerSec <= 1.0f) return -1.0f;   // effectively not moving -> unknown
//     // return RemainingLength / SpeedUUPerSec;
//   static bool MakingProgress(float PrevDistRemaining, float CurDistRemaining, float MinDelta);
//     // return (PrevDistRemaining - CurDistRemaining) >= MinDelta;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeNavEta.h"

BEGIN_DEFINE_SPEC(FEmergeNavEtaSpec, "Emergence.Nav.Eta",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNavEtaSpec)
void FEmergeNavEtaSpec::Define()
{
	Describe("EtaSeconds", [this]()
	{
		It("distance / speed, guards near-zero speed", [this]()
		{
			TestEqual(TEXT("1500 / 375 -> 4s"), UEmergeNavEta::EtaSeconds(1500.f, 375.f), 4.0f);
			TestEqual(TEXT("0 remaining -> 0"), UEmergeNavEta::EtaSeconds(0.f, 375.f), 0.0f);
			TestEqual(TEXT("not moving -> -1"), UEmergeNavEta::EtaSeconds(1500.f, 0.5f), -1.0f);
		});
	});
	Describe("MakingProgress", [this]()
	{
		It("true only when closing by >= MinDelta", [this]()
		{
			TestTrue(TEXT("1000->800 by 50 -> progress"), UEmergeNavEta::MakingProgress(1000.f, 800.f, 50.f));
			TestFalse(TEXT("1000->990 by 50 -> stalled"), UEmergeNavEta::MakingProgress(1000.f, 990.f, 50.f));
			TestFalse(TEXT("moving away -> false"), UEmergeNavEta::MakingProgress(800.f, 1000.f, 50.f));
		});
	});
}
#endif
