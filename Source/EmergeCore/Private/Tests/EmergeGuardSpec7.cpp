#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec7, "Emergence.Guardian.Check7",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec7)

void FEmergeGuardSpec7::Define()
{
    Describe("The Emergence Guardian functionality", [this]()
    {
        It("verifies a simple deterministic fact using UE functions", [this]()
        {
            TestTrue(TEXT("FMath::Abs(-5) == 5"), FMath::Abs(-5) == 5);
        });
    });
}
#endif