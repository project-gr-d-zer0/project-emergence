// The per-phase EVALUATOR runs NAMED Automation Specs headless (UnrealEditor-Cmd ... -unattended -NullRHI,
// or Gauntlet RunUnreal -runtest=...). Each It() is an isolatable, execution-based (non-fabricable) gate.
// Real Phase-0 exit-criteria become specs like "Emergence.Ballistics.800mMatchesCalculator".
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
BEGIN_DEFINE_SPEC(FEmergeSmokeSpec, "Emergence.Smoke.Boots",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSmokeSpec)
void FEmergeSmokeSpec::Define()
{
    Describe("The Emergence scaffold", [this]()
    {
        It("compiles and the test harness runs", [this]()
        {
            TestTrue(TEXT("sanity: 2+2==4"), 2 + 2 == 4);
        });
    });
}
#endif
