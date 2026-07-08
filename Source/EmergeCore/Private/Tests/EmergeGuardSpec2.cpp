#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec2, "Emergence.Guardian.Check2",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec2)

void FEmergeGuardSpec2::Define()
{
    Describe("The Emergence guardian", [this]()
    {
        It("verifies a simple deterministic fact using UE functions", [this]()
        {
            TestTrue(TEXT("FMath::Clamp(5, 1, 10) == 5"), FMath::Clamp(5, 1, 10) == 5);
        });
    });
}
#endif