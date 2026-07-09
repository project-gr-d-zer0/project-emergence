// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: trade listing fee (Tarkov-lesson).
// Fees grow superlinearly as asking price diverges above fair value - structurally punishes market
// manipulation from day one.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeEconomy (do NOT rename/duplicate it;
//   keep ItemValue and RepairCost exactly), in the existing
//   Public/Economy/EmergeEconomy.h + Private/Economy/EmergeEconomy.cpp:
//   static int32 TradeFee(int32 FairValue, int32 AskingPrice);
//     // if (FairValue <= 0 || AskingPrice <= 0) return 0;
//     // const float Ratio = (float)AskingPrice / (float)FairValue;
//     // return FMath::RoundToInt(AskingPrice * 0.05f * FMath::Max(1.0f, Ratio));
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Economy/EmergeEconomy.h"

BEGIN_DEFINE_SPEC(FEmergeTradeFeeSpec, "Emergence.Economy.TradeFee",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeTradeFeeSpec)
void FEmergeTradeFeeSpec::Define()
{
	Describe("TradeFee (5% base, superlinear above fair value)", [this]()
	{
		It("matches exact golden values and guards", [this]()
		{
			TestEqual(TEXT("fair ask 100/100 -> 5"), UEmergeEconomy::TradeFee(100, 100), 5);
			TestEqual(TEXT("double ask 200/100 -> 20"), UEmergeEconomy::TradeFee(100, 200), 20);
			TestEqual(TEXT("4x gouge 400/100 -> 80"), UEmergeEconomy::TradeFee(100, 400), 80);
			TestEqual(TEXT("underpricing keeps base 5%: 50/100 -> 3"), UEmergeEconomy::TradeFee(100, 50), 3);
			TestEqual(TEXT("zero value -> 0"), UEmergeEconomy::TradeFee(0, 100), 0);
		});
	});
}
#endif
