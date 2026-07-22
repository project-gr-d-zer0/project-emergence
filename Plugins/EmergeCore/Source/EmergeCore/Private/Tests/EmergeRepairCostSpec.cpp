// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: repair cost.
// Connects durability <-> economy: repairing condition costs half the value of the condition restored.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeEconomy (do NOT rename/duplicate;
//   keep ItemValue exactly), in the existing
//   Public/Economy/EmergeEconomy.h + Private/Economy/EmergeEconomy.cpp:
//   static int32 RepairCost(int32 BaseValue, float FromConditionPct, float ToConditionPct);
//     // const float F = FMath::Clamp(FromConditionPct, 0.0f, 100.0f);
//     // const float T = FMath::Clamp(ToConditionPct,   0.0f, 100.0f);
//     // if (T <= F) return 0;
//     // return FMath::RoundToInt(BaseValue * ((T - F) / 100.0f) * 0.5f);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Economy/EmergeEconomy.h"

BEGIN_DEFINE_SPEC(FEmergeRepairCostSpec, "Emergence.Economy.RepairCost",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeRepairCostSpec)
void FEmergeRepairCostSpec::Define()
{
	Describe("RepairCost (half the restored value)", [this]()
	{
		It("matches exact golden values and guards no-op repairs", [this]()
		{
			TestEqual(TEXT("no restore -> 0"),        UEmergeEconomy::RepairCost(100, 20.f, 20.f), 0);
			TestEqual(TEXT("full 0->100 of 100 -> 50"),UEmergeEconomy::RepairCost(100, 0.f, 100.f), 50);
			TestEqual(TEXT("half 50->100 of 200 -> 50"),UEmergeEconomy::RepairCost(200, 50.f, 100.f), 50);
			TestEqual(TEXT("downgrade -> 0"),         UEmergeEconomy::RepairCost(100, 80.f, 60.f), 0);
		});
	});
}
#endif
