// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: decide what a stuck path-follower does.
// Returns an action code: 0=Continue, 1=Repath, 3=Abandon. Deterministic ladder.
// CONTRACT — NEW class UEmergeNavRecovery : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeNavRecovery.h + Private/Nav/EmergeNavRecovery.cpp, static UFUNCTIONs:
//   static int32 DecideRecovery(float StuckSeconds, int32 RepathCount, float StuckThreshold, int32 MaxRepaths);
//     // if (StuckSeconds < StuckThreshold) return 0;   // Continue - not stuck long enough
//     // if (RepathCount < MaxRepaths)      return 1;   // Repath - try a fresh path
//     // return 3;                                      // Abandon - out of repaths
//   static bool ShouldGiveUp(int32 ActionCode);   // return ActionCode == 3;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeNavRecovery.h"

BEGIN_DEFINE_SPEC(FEmergeNavRecoverySpec, "Emergence.Nav.Recovery",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNavRecoverySpec)
void FEmergeNavRecoverySpec::Define()
{
	Describe("DecideRecovery ladder", [this]()
	{
		It("continue / repath / abandon by exact thresholds", [this]()
		{
			TestEqual(TEXT("not stuck -> Continue"), UEmergeNavRecovery::DecideRecovery(1.0f, 0, 3.0f, 2), 0);
			TestEqual(TEXT("stuck, repaths left -> Repath"), UEmergeNavRecovery::DecideRecovery(3.0f, 0, 3.0f, 2), 1);
			TestEqual(TEXT("stuck, one repath used -> Repath"), UEmergeNavRecovery::DecideRecovery(4.0f, 1, 3.0f, 2), 1);
			TestEqual(TEXT("stuck, out of repaths -> Abandon"), UEmergeNavRecovery::DecideRecovery(4.0f, 2, 3.0f, 2), 3);
		});
	});
	Describe("ShouldGiveUp", [this]()
	{
		It("only on Abandon", [this]()
		{
			TestTrue(TEXT("3 -> give up"), UEmergeNavRecovery::ShouldGiveUp(3));
			TestFalse(TEXT("0 -> no"), UEmergeNavRecovery::ShouldGiveUp(0));
			TestFalse(TEXT("1 -> no"), UEmergeNavRecovery::ShouldGiveUp(1));
		});
	});
}
#endif
