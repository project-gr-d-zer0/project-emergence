// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: staged wound infection (DayZ model).
// Untreated dirty wounds infect on a timer: stage 1 (curable ONLY by disinfectant) -> stage 2 (fever,
// needs antibiotics) -> stage 3 (critical). The delayed-consequence loop: bandage with a dirty rag at
// minute 0, die at depth 3 twenty minutes later.
// CONTRACT — NEW class UEmergeInfection : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeInfection.h + Private/Survival/EmergeInfection.cpp, static UFUNCTIONs:
//   static int32 StageForMinutes(float MinutesSinceWound, bool bWoundClean);
//     // if (bWoundClean) return 0;
//     // if (MinutesSinceWound < 20.0f) return 0;
//     // if (MinutesSinceWound < 40.0f) return 1;
//     // if (MinutesSinceWound < 60.0f) return 2;
//     // return 3;
//   static bool ItemCuresStage(int32 ItemType, int32 Stage);
//     // disinfectant = 3 cures stage 1 only; antibiotics = 4 cure stage 2 only; nothing cures stage 3
//     // return (Stage == 1 && ItemType == 3) || (Stage == 2 && ItemType == 4);
//   static float StaminaRegenMultiplierForStage(int32 Stage);
//     // stage 0 -> 1.0; stage 1 -> 0.75; stage 2 -> 0.5; stage 3+ -> 0.25
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeInfection.h"

BEGIN_DEFINE_SPEC(FEmergeInfectionSpec, "Emergence.Survival.Infection",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInfectionSpec)
void FEmergeInfectionSpec::Define()
{
	Describe("StageForMinutes (clean wounds never infect)", [this]()
	{
		It("stages at 20/40/60 minutes for dirty wounds only", [this]()
		{
			TestEqual(TEXT("clean at 60m -> 0"), UEmergeInfection::StageForMinutes(60.f, true), 0);
			TestEqual(TEXT("dirty 19m -> 0"), UEmergeInfection::StageForMinutes(19.f, false), 0);
			TestEqual(TEXT("dirty 20m -> 1"), UEmergeInfection::StageForMinutes(20.f, false), 1);
			TestEqual(TEXT("dirty 40m -> 2"), UEmergeInfection::StageForMinutes(40.f, false), 2);
			TestEqual(TEXT("dirty 60m -> 3"), UEmergeInfection::StageForMinutes(60.f, false), 3);
		});
	});

	Describe("ItemCuresStage (right medicine, right window)", [this]()
	{
		It("disinfectant only for stage 1, antibiotics only for stage 2", [this]()
		{
			TestTrue(TEXT("disinfectant cures s1"), UEmergeInfection::ItemCuresStage(3, 1));
			TestFalse(TEXT("antibiotics useless at s1"), UEmergeInfection::ItemCuresStage(4, 1));
			TestTrue(TEXT("antibiotics cure s2"), UEmergeInfection::ItemCuresStage(4, 2));
			TestFalse(TEXT("disinfectant useless at s2"), UEmergeInfection::ItemCuresStage(3, 2));
			TestFalse(TEXT("nothing cures s3"), UEmergeInfection::ItemCuresStage(4, 3));
		});
	});

	Describe("StaminaRegenMultiplierForStage", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("s0 -> 1.0"), UEmergeInfection::StaminaRegenMultiplierForStage(0), 1.f);
			TestEqual(TEXT("s1 -> 0.75"), UEmergeInfection::StaminaRegenMultiplierForStage(1), 0.75f);
			TestEqual(TEXT("s2 -> 0.5"), UEmergeInfection::StaminaRegenMultiplierForStage(2), 0.5f);
			TestEqual(TEXT("s3 -> 0.25"), UEmergeInfection::StaminaRegenMultiplierForStage(3), 0.25f);
		});
	});
}
#endif
