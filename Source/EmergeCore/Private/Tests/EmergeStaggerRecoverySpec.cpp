// HUMAN-AUTHORED trusted acceptance spec. Feature: stagger meter natural recovery.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeStagger.h"

BEGIN_DEFINE_SPEC(FEmergeStaggerRecoverySpec, "Emergence.Combat.StaggerRecovery",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaggerRecoverySpec)
void FEmergeStaggerRecoverySpec::Define()
{
	Describe("MeterAfterRecovery (decays toward 0)", [this]()
	{
		It("subtracts rate*dt, floors at 0, guards negatives", [this]()
		{
			TestEqual(TEXT("100 - 20*3 -> 40"), UEmergeStagger::MeterAfterRecovery(100.f, 20.f, 3.f), 40.f);
			TestEqual(TEXT("over-recover floors at 0"), UEmergeStagger::MeterAfterRecovery(10.f, 20.f, 3.f), 0.f);
			TestEqual(TEXT("negative dt -> unchanged"), UEmergeStagger::MeterAfterRecovery(50.f, 20.f, -1.f), 50.f);
		});
	});
}
#endif
