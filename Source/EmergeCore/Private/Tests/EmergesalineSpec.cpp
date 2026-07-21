// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: saline / rehydration IV (DayZ-style).
// A saline bag boosts blood AND water regen while it drains - the medical counter to blood loss / dehydration.
// Composes with EmergeBloodRegen. Numbers exact from decompiled DayZ playerconstants.c: SALINE_BLOOD_REGEN_PER_SEC
// 3, SALINE_WATER_REGEN_PER_SEC 2.1, SALINE_LIQUID_AMOUNT 500.
// CONTRACT — NEW class UEmergeSaline : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeSaline.h + Private/Survival/EmergeSaline.cpp, static UFUNCTIONs.
//
//   static float BloodRegenPerTick(float DeltaT);   // return 3.0f * DeltaT;   // SALINE_BLOOD_REGEN_PER_SEC
//   static float WaterRegenPerTick(float DeltaT);    // return 2.1f * DeltaT;   // SALINE_WATER_REGEN_PER_SEC
//   static float LiquidAmount();                     // return 500.0f;          // SALINE_LIQUID_AMOUNT
//   static float BloodRestoredOver(float Seconds);   // return 3.0f * Seconds;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeSaline.h"

BEGIN_DEFINE_SPEC(FEmergeSalineSpec, "Emergence.Survival.Saline",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSalineSpec)
void FEmergeSalineSpec::Define()
{
	Describe("Regen rates & amount", [this]()
	{
		It("boosts blood and water while draining (exact DayZ values)", [this]()
		{
			TestEqual(TEXT("blood 3/s"), UEmergeSaline::BloodRegenPerTick(1.0f), 3.0f);
			TestEqual(TEXT("blood dt=2 -> 6"), UEmergeSaline::BloodRegenPerTick(2.0f), 6.0f);
			TestEqual(TEXT("water 2.1/s"), UEmergeSaline::WaterRegenPerTick(1.0f), 2.1f);
			TestEqual(TEXT("water dt=10 -> 21"), UEmergeSaline::WaterRegenPerTick(10.0f), 21.0f);
			TestEqual(TEXT("liquid amount 500"), UEmergeSaline::LiquidAmount(), 500.0f);
			TestEqual(TEXT("blood restored over 60s -> 180"), UEmergeSaline::BloodRestoredOver(60.0f), 180.0f);
		});
	});
}
#endif
