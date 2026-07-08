#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec3, "Emergence.Guardian.Check3",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec3)

void FEmergeGuardSpec3::Define()
{
    Describe("The Emergence guardian functionality", [this]()
    {
        It("verifies a simple deterministic fact using UE functions", [this]()
        {
            TestEqual(TEXT("FMath::Clamp(5, 1, 10) == 5"), FMath::Clamp(5, 1, 10), 5);
        });
    });
}
#endif