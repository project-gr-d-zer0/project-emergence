// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: firewood gathering (survival).
// Chopping firewood yields logs by tool tier and takes time accordingly; each log is fuel for EmergeCampfire.
// TUNABLE design values, fixed deterministic model.
// CONTRACT — NEW class UEmergeFirewood : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeFirewood.h + Private/Survival/EmergeFirewood.cpp, static UFUNCTIONs.
//   ToolTier int32: 0 Hands, 1 Knife, 2 Hatchet, 3 Saw.
//
//   static int32 YieldForTool(int32 ToolTier);
//     // switch(ToolTier){ case 1: return 2; case 2: return 4; case 3: return 6; default: return 1; }
//   static float GatherTimeSec(int32 ToolTier);
//     // switch(ToolTier){ case 1: return 20.0f; case 2: return 10.0f; case 3: return 6.0f; default: return 30.0f; }
//   static float BurnValuePerLog();   // return 60.0f;   // seconds of campfire fuel per log
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeFirewood.h"

BEGIN_DEFINE_SPEC(FEmergeFirewoodSpec, "Emergence.Survival.Firewood",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFirewoodSpec)
void FEmergeFirewoodSpec::Define()
{
	Describe("Firewood by tool", [this]()
	{
		It("better tools yield more, faster", [this]()
		{
			TestEqual(TEXT("hands 1 log"), UEmergeFirewood::YieldForTool(0), 1);
			TestEqual(TEXT("knife 2"), UEmergeFirewood::YieldForTool(1), 2);
			TestEqual(TEXT("hatchet 4"), UEmergeFirewood::YieldForTool(2), 4);
			TestEqual(TEXT("saw 6"), UEmergeFirewood::YieldForTool(3), 6);
			TestEqual(TEXT("unknown -> hands"), UEmergeFirewood::YieldForTool(9), 1);
			TestEqual(TEXT("hands slow 30s"), UEmergeFirewood::GatherTimeSec(0), 30.0f);
			TestEqual(TEXT("hatchet 10s"), UEmergeFirewood::GatherTimeSec(2), 10.0f);
			TestEqual(TEXT("log burns 60s"), UEmergeFirewood::BurnValuePerLog(), 60.0f);
		});
	});
}
#endif
