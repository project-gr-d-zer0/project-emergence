#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeGuardSpec1, "Emergence.Guardian.Check1",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGuardSpec1)

void FEmergeGuardSpec1::Define()
{
    Describe("The Emergence guardian checks", [this]()
    {
        It("verifies a simple mathematical fact using UE5.8 FMath functions", [this]()
        {
            const float Value = 50.0f;
            const float ClampedValue = FMath::Clamp(Value, 0.0f, 100.0f);
            TestTrue(TEXT("FMath::Clamp works as expected"), ClampedValue == 50.0f);
        });
    });
}
#endif