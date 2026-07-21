// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weight/encumbrance HUD view-model (DayZ-style).
// SHELL stratum: shows carried weight and the stamina it costs. Below the free threshold there is no penalty;
// above it, each kg cuts max stamina, floored so you're never fully locked. Numbers exact from decompiled DayZ
// constants.c StaminaHandlerCfg: STAMINA_WEIGHT_LIMIT_THRESHOLD 6000 g (no penalty at/below), STAMINA_KG_TO_GRAMS
// 1000, STAMINA_KG_TO_STAMINAPERCENT_PENALTY 1.75 per kg, STAMINA_MAX 100, STAMINA_MIN_CAP 5.
// CONTRACT — NEW class UEmergeWeightHud : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeWeightHud.h + Private/Shell/EmergeWeightHud.cpp, static UFUNCTIONs.
//
//   static float PenalizedWeightKg(float WeightGrams);
//     // weight above the free threshold, in kg. return FMath::Max(0.0f, (WeightGrams - 6000.0f) / 1000.0f);
//   static float StaminaPenalty(float WeightGrams);
//     // return PenalizedWeightKg(WeightGrams) * 1.75f;
//   static float MaxStaminaForWeight(float WeightGrams);
//     // return FMath::Clamp(100.0f - StaminaPenalty(WeightGrams), 5.0f, 100.0f);
//   static bool  IsOverloaded(float WeightGrams);     // return WeightGrams > 6000.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeWeightHud.h"

BEGIN_DEFINE_SPEC(FEmergeWeightHudSpec, "Emergence.Shell.WeightHud",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeightHudSpec)
void FEmergeWeightHudSpec::Define()
{
	Describe("PenalizedWeightKg & penalty", [this]()
	{
		It("only weight above 6kg is penalized, at 1.75/kg", [this]()
		{
			TestEqual(TEXT("5kg -> 0 penalized"), UEmergeWeightHud::PenalizedWeightKg(5000.f), 0.0f);
			TestEqual(TEXT("6kg (threshold) -> 0"), UEmergeWeightHud::PenalizedWeightKg(6000.f), 0.0f);
			TestEqual(TEXT("7kg -> 1kg over"), UEmergeWeightHud::PenalizedWeightKg(7000.f), 1.0f);
			TestEqual(TEXT("10kg -> 4kg over"), UEmergeWeightHud::PenalizedWeightKg(10000.f), 4.0f);
			TestEqual(TEXT("7kg penalty 1.75"), UEmergeWeightHud::StaminaPenalty(7000.f), 1.75f);
			TestEqual(TEXT("10kg penalty 7.0"), UEmergeWeightHud::StaminaPenalty(10000.f), 7.0f);
		});
	});

	Describe("MaxStaminaForWeight & overload flag", [this]()
	{
		It("max stamina drops with load, floored at 5", [this]()
		{
			TestEqual(TEXT("unloaded -> 100"), UEmergeWeightHud::MaxStaminaForWeight(5000.f), 100.0f);
			TestEqual(TEXT("7kg -> 98.25"), UEmergeWeightHud::MaxStaminaForWeight(7000.f), 98.25f);
			TestEqual(TEXT("10kg -> 93"), UEmergeWeightHud::MaxStaminaForWeight(10000.f), 93.0f);
			TestEqual(TEXT("massive load floors at 5"), UEmergeWeightHud::MaxStaminaForWeight(100000.f), 5.0f);
		});
		It("overloaded once above the free threshold", [this]()
		{
			TestFalse(TEXT("6kg not overloaded"), UEmergeWeightHud::IsOverloaded(6000.f));
			TestTrue(TEXT("6.001kg overloaded"), UEmergeWeightHud::IsOverloaded(6001.f));
			TestFalse(TEXT("light not overloaded"), UEmergeWeightHud::IsOverloaded(3000.f));
		});
	});
}
#endif
