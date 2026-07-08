#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec2, "Emergence.Guardian.Check2",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec2)

void FEmergeGuardSpec2::Define()
{
    Describe("The Emergence guardian checks", [this]()
    {
        It("verifies a simple math operation using FMath::Max", [this]()
        {
            const int32 MaxValue = FMath::Max(10, 5);
            TestEqual(TEXT("FMath::Max returns the correct value"), MaxValue, 10);
        });
    });
}
#endif