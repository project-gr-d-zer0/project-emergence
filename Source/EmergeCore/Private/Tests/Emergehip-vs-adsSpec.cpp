// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hip vs ADS accuracy/mobility (combat/gunplay).
// Aiming down sights tightens accuracy but slows movement; hip-fire is the opposite. Composes with
// EmergeSpreadStance / EmergeStanceIndicator. TUNABLE design multipliers, fixed deterministic model.
// CONTRACT — NEW class UEmergeAimMode : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeAimMode.h + Private/Combat/EmergeAimMode.cpp, static UFUNCTIONs.
//
//   static float AccuracyMultiplier(bool bAds);    // return bAds ? 0.4f : 1.0f;   // ADS tightens spread
//   static float MoveSpeedMultiplier(bool bAds);    // return bAds ? 0.7f : 1.0f;   // ADS slows you
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeAimMode.h"

BEGIN_DEFINE_SPEC(FEmergeAimModeSpec, "Emergence.Combat.AimMode",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAimModeSpec)
void FEmergeAimModeSpec::Define()
{
	Describe("Hip vs ADS", [this]()
	{
		It("ADS tightens spread and slows movement", [this]()
		{
			TestEqual(TEXT("ADS accuracy 0.4"), UEmergeAimMode::AccuracyMultiplier(true), 0.4f);
			TestEqual(TEXT("hipfire accuracy 1.0"), UEmergeAimMode::AccuracyMultiplier(false), 1.0f);
			TestEqual(TEXT("ADS move 0.7"), UEmergeAimMode::MoveSpeedMultiplier(true), 0.7f);
			TestEqual(TEXT("hipfire move 1.0"), UEmergeAimMode::MoveSpeedMultiplier(false), 1.0f);
		});
	});
}
#endif
