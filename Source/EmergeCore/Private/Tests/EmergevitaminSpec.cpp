// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: vitamins / immune boost (DayZ-style).
// Vitamins temporarily max out the immune system (GetImmunityLevel returns GREAT while boosted), fighting off
// disease agents from EmergeDisease. Numbers exact from decompiled DayZ playerconstants.c VITAMINS_LIFETIME_SECS
// 300; EStatLevels GREAT ordinal is 0.
// CONTRACT — NEW class UEmergeVitamin : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeVitamin.h + Private/Survival/EmergeVitamin.cpp, static UFUNCTIONs.
//
//   static float LifetimeSecs();                 // return 300.0f;   // VITAMINS_LIFETIME_SECS
//   static float RemainingSecs(float Elapsed);    // return FMath::Max(0.0f, 300.0f - Elapsed);
//   static bool  IsActive(float Elapsed);         // return Elapsed < 300.0f;
//   static int32 BoostedImmunityLevel();          // return 0;   // EStatLevels.GREAT while boosted
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeVitamin.h"

BEGIN_DEFINE_SPEC(FEmergeVitaminSpec, "Emergence.Survival.Vitamin",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeVitaminSpec)
void FEmergeVitaminSpec::Define()
{
	Describe("Vitamin lifetime & boost", [this]()
	{
		It("300s of maxed immunity", [this]()
		{
			TestEqual(TEXT("lifetime 300"), UEmergeVitamin::LifetimeSecs(), 300.0f);
			TestEqual(TEXT("100s in -> 200 left"), UEmergeVitamin::RemainingSecs(100.0f), 200.0f);
			TestEqual(TEXT("expired -> 0"), UEmergeVitamin::RemainingSecs(300.0f), 0.0f);
			TestEqual(TEXT("past expiry clamps 0"), UEmergeVitamin::RemainingSecs(400.0f), 0.0f);
			TestTrue(TEXT("active at 100s"), UEmergeVitamin::IsActive(100.0f));
			TestFalse(TEXT("inactive at 300s"), UEmergeVitamin::IsActive(300.0f));
			TestEqual(TEXT("boosted immunity is GREAT (0)"), UEmergeVitamin::BoostedImmunityLevel(), 0);
		});
	});
}
#endif
