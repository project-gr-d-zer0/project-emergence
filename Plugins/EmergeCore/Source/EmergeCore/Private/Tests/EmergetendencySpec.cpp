// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: DayZ status-strip tendency level.
// The DayZ vitals strip shows a stat's rate-of-change as directional arrows (1-3 up or down). This is the
// pure classifier behind that: a per-tick value delta -> a signed tendency level for the HUD arrows.
// Deterministic, exact goldens, boundaries covered. Serves the always-on strip in the health UI.
// CONTRACT — NEW class UEmergeTendency : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeTendency.h + Private/Survival/EmergeTendency.cpp, one static UFUNCTION.
//   static int32 TendencyLevel(float Delta);   // signed arrow level -3..+3:
//     if abs(Delta) < 0.5 -> 0 (stable, no arrow);
//     magnitude = abs(Delta) < 2.0 -> 1, < 5.0 -> 2, else 3;  sign = Delta > 0 ? + : - .
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeTendency.h"

BEGIN_DEFINE_SPEC(FEmergeTendencySpec, "Emergence.Survival.Tendency",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeTendencySpec)
void FEmergeTendencySpec::Define()
{
    Describe("TendencyLevel (signed DayZ arrow level, deadband + 3 magnitudes)", [this]()
    {
        It("classifies rate-of-change into -3..+3 with boundaries", [this]()
        {
            TestEqual(TEXT("zero -> stable"),       UEmergeTendency::TendencyLevel(0.f),    0);
            TestEqual(TEXT("0.49 in deadband"),     UEmergeTendency::TendencyLevel(0.49f),  0);
            TestEqual(TEXT("-0.2 in deadband"),     UEmergeTendency::TendencyLevel(-0.2f),  0);
            TestEqual(TEXT("0.5 -> +1"),            UEmergeTendency::TendencyLevel(0.5f),   1);
            TestEqual(TEXT("1.9 -> +1"),            UEmergeTendency::TendencyLevel(1.9f),   1);
            TestEqual(TEXT("2.0 -> +2"),            UEmergeTendency::TendencyLevel(2.0f),   2);
            TestEqual(TEXT("4.9 -> +2"),            UEmergeTendency::TendencyLevel(4.9f),   2);
            TestEqual(TEXT("5.0 -> +3"),            UEmergeTendency::TendencyLevel(5.0f),   3);
            TestEqual(TEXT("-1.0 -> -1"),           UEmergeTendency::TendencyLevel(-1.0f), -1);
            TestEqual(TEXT("-3.0 -> -2"),           UEmergeTendency::TendencyLevel(-3.0f), -2);
            TestEqual(TEXT("-6.0 -> -3"),           UEmergeTendency::TendencyLevel(-6.0f), -3);
        });
    });
}
#endif
