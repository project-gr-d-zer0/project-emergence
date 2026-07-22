// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hunger/thirst need icon (shell).
// SHELL stratum: the need-status icon state (ok/low/warning/critical) for a normalized need value, and whether
// it should flash at critical. Pairs with EmergeMetabolism / EmergeVitalHud. Pure view banding; deterministic.
// CONTRACT — NEW class UEmergeNeedIcon : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeNeedIcon.h + Private/Shell/EmergeNeedIcon.cpp, static UFUNCTIONs.
//   State int32: 0 Ok, 1 Low, 2 Warning, 3 Critical.
//
//   static int32 IconState(float Value01);
//     // if (Value01 > 0.6f) return 0;  if (Value01 > 0.3f) return 1;  if (Value01 > 0.1f) return 2;  return 3;
//   static bool  ShouldFlash(float Value01);   // return Value01 <= 0.1f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeNeedIcon.h"

BEGIN_DEFINE_SPEC(FEmergeNeedIconSpec, "Emergence.Shell.NeedIcon",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNeedIconSpec)
void FEmergeNeedIconSpec::Define()
{
	Describe("IconState & flash", [this]()
	{
		It("bands the need value and flashes at critical", [this]()
		{
			TestEqual(TEXT("0.8 ok"), UEmergeNeedIcon::IconState(0.8f), 0);
			TestEqual(TEXT("0.5 low"), UEmergeNeedIcon::IconState(0.5f), 1);
			TestEqual(TEXT("0.3 warning"), UEmergeNeedIcon::IconState(0.3f), 2);
			TestEqual(TEXT("0.2 warning"), UEmergeNeedIcon::IconState(0.2f), 2);
			TestEqual(TEXT("0.1 critical"), UEmergeNeedIcon::IconState(0.1f), 3);
			TestEqual(TEXT("0.05 critical"), UEmergeNeedIcon::IconState(0.05f), 3);
			TestTrue(TEXT("0.1 flashes"), UEmergeNeedIcon::ShouldFlash(0.1f));
			TestFalse(TEXT("0.2 no flash"), UEmergeNeedIcon::ShouldFlash(0.2f));
		});
	});
}
#endif
