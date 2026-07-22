// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: drowning (DayZ-style).
// Deep water past a submersion threshold puts the player in a drowning state (a higher bar while actively
// swimming, a much lower one when unconscious/face-down); air-bubble feedback speeds up as stamina drains.
// Numbers exact from decompiled DayZ playerconstants.c: DROWNING_SWIMMING_THRESHOLD 0.4, DROWNING_UNCONSCIOUS_
// THRESHOLD 0.1, DROWNING_BUBBLE_FREQUENCY_MAX 4000 ms (at highest stamina), _MIN 1000 ms (at lowest stamina).
// CONTRACT — NEW class UEmergeDrowning : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeDrowning.h + Private/Survival/EmergeDrowning.cpp, static UFUNCTIONs.
//
//   static float SwimmingThreshold();     // return 0.4f;   // DROWNING_SWIMMING_THRESHOLD
//   static float UnconsciousThreshold();  // return 0.1f;   // DROWNING_UNCONSCIOUS_THRESHOLD
//   static bool  IsDrowning(float Submersion01, float Threshold);   // return Submersion01 >= Threshold;
//   static float BubbleIntervalMs(float Stamina01);
//     // bubbles are infrequent (4000ms) at full stamina, frequent (1000ms) when spent:
//     // return FMath::Lerp(1000.0f, 4000.0f, FMath::Clamp(Stamina01, 0.0f, 1.0f));
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeDrowning.h"

BEGIN_DEFINE_SPEC(FEmergeDrowningSpec, "Emergence.Survival.Drowning",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDrowningSpec)
void FEmergeDrowningSpec::Define()
{
	Describe("Drowning state", [this]()
	{
		It("submersion past the threshold means drowning (swimming vs unconscious bars)", [this]()
		{
			TestEqual(TEXT("swimming threshold 0.4"), UEmergeDrowning::SwimmingThreshold(), 0.4f);
			TestEqual(TEXT("unconscious threshold 0.1"), UEmergeDrowning::UnconsciousThreshold(), 0.1f);
			TestTrue(TEXT("0.5 swimming -> drowning"), UEmergeDrowning::IsDrowning(0.5f, UEmergeDrowning::SwimmingThreshold()));
			TestTrue(TEXT("exactly 0.4 -> drowning"), UEmergeDrowning::IsDrowning(0.4f, UEmergeDrowning::SwimmingThreshold()));
			TestFalse(TEXT("0.3 swimming -> safe"), UEmergeDrowning::IsDrowning(0.3f, UEmergeDrowning::SwimmingThreshold()));
			TestTrue(TEXT("0.15 unconscious -> drowning"), UEmergeDrowning::IsDrowning(0.15f, UEmergeDrowning::UnconsciousThreshold()));
			TestFalse(TEXT("0.05 unconscious -> safe"), UEmergeDrowning::IsDrowning(0.05f, UEmergeDrowning::UnconsciousThreshold()));
		});
	});

	Describe("BubbleIntervalMs", [this]()
	{
		It("infrequent at full stamina, frequent when spent", [this]()
		{
			TestEqual(TEXT("full stamina -> 4000ms"), UEmergeDrowning::BubbleIntervalMs(1.0f), 4000.0f);
			TestEqual(TEXT("empty stamina -> 1000ms"), UEmergeDrowning::BubbleIntervalMs(0.0f), 1000.0f);
			TestEqual(TEXT("half -> 2500ms"), UEmergeDrowning::BubbleIntervalMs(0.5f), 2500.0f);
			TestEqual(TEXT("over 1 clamps -> 4000ms"), UEmergeDrowning::BubbleIntervalMs(1.5f), 4000.0f);
		});
	});
}
#endif
