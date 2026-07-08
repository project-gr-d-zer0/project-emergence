#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec5, "Emergence.Guardian.Check5",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec5)

void FEmergeGuardSpec5::Define()
{
    Describe("The Emergence Guardian functionality", [this]()
    {
        It("verifies a simple deterministic fact using UE functions", [this]()
        {
            TestTrue(TEXT("FMath::Abs(-1) == 1"), FMath::Abs(-1) == 1);
        });
    });
}
#endif