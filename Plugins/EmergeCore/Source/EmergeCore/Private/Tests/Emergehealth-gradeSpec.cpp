// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: health verbal grade (PZ/DayZ health panel).
// The DayZ/PZ health UI shows overall health as a WORD, never a number (PZ getDamageStatusString). This is the
// pure mapping behind that. Deterministic, exact goldens, boundaries covered. Serves the vertical-slice health panel.
// CONTRACT — NEW class UEmergeHealthGrade : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeHealthGrade.h + Private/Survival/EmergeHealthGrade.cpp, one static UFUNCTION.
//   static FString GradeFor(float Health);   // Health 0..100 -> verbal band, inclusive lower bounds:
//     >=95 "OK"; >=80 "Minor"; >=60 "Moderate"; >=35 "Severe"; >0 "Critical"; else (<=0) "Terminal".
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeHealthGrade.h"

BEGIN_DEFINE_SPEC(FEmergeHealthGradeSpec, "Emergence.Survival.HealthGrade",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHealthGradeSpec)
void FEmergeHealthGradeSpec::Define()
{
    Describe("GradeFor (PZ verbal bands, no numbers)", [this]()
    {
        It("maps health to the correct band including boundaries", [this]()
        {
            TestEqual(TEXT("full -> OK"),        UEmergeHealthGrade::GradeFor(100.f), FString(TEXT("OK")));
            TestEqual(TEXT("95 boundary -> OK"), UEmergeHealthGrade::GradeFor(95.f),  FString(TEXT("OK")));
            TestEqual(TEXT("94 -> Minor"),       UEmergeHealthGrade::GradeFor(94.f),  FString(TEXT("Minor")));
            TestEqual(TEXT("80 -> Minor"),       UEmergeHealthGrade::GradeFor(80.f),  FString(TEXT("Minor")));
            TestEqual(TEXT("79 -> Moderate"),    UEmergeHealthGrade::GradeFor(79.f),  FString(TEXT("Moderate")));
            TestEqual(TEXT("60 -> Moderate"),    UEmergeHealthGrade::GradeFor(60.f),  FString(TEXT("Moderate")));
            TestEqual(TEXT("35 -> Severe"),      UEmergeHealthGrade::GradeFor(35.f),  FString(TEXT("Severe")));
            TestEqual(TEXT("34 -> Critical"),    UEmergeHealthGrade::GradeFor(34.f),  FString(TEXT("Critical")));
            TestEqual(TEXT("0.5 -> Critical"),   UEmergeHealthGrade::GradeFor(0.5f),  FString(TEXT("Critical")));
            TestEqual(TEXT("0 -> Terminal"),     UEmergeHealthGrade::GradeFor(0.f),   FString(TEXT("Terminal")));
            TestEqual(TEXT("negative -> Terminal"), UEmergeHealthGrade::GradeFor(-5.f), FString(TEXT("Terminal")));
        });
    });
}
#endif
