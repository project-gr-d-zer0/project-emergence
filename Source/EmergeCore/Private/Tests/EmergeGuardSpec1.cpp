#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec1, "Emergence.Guardian.Check1",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec1)

void FEmergeGuardSpec1::Define()
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