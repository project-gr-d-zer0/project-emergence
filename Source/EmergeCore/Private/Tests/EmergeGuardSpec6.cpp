#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec6, "Emergence.Guardian.Check6",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec6)

void FEmergeGuardSpec6::Define()
{
    Describe("The Emergence guardian functionality", [this]()
    {
        It("verifies a simple deterministic fact using UE functions", [this]()
        {
            const float Value = 5.0f;
            const float ClampedValue = FMath::Clamp(Value, 0.0f, 10.0f);
            TestTrue(TEXT("FMath::Clamp works as expected"), ClampedValue == 5.0f);
        });
    });
}
#endif