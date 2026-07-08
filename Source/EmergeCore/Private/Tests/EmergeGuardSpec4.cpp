#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec4, "Emergence.Guardian.Check4",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec4)

void FEmergeGuardSpec4::Define()
{
    Describe("The Emergence guardian functionality", [this]()
    {
        It("verifies a simple deterministic fact using UE functions", [this]()
        {
            TestTrue(TEXT("FMath::Clamp(5, 0, 10) == 5"), FMath::Clamp(5, 0, 10) == 5);
        });
    });
}
#endif