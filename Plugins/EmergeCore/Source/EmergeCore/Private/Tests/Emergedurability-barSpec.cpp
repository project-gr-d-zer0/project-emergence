// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: equipment durability bar (shell).
// SHELL stratum: the item-durability bar - fill fraction, a green/yellow/red color band, and a broken flag.
// Pairs with EmergeArmorWear / EmergeRepairCost. Pure view banding; deterministic.
// CONTRACT — NEW class UEmergeDurabilityBar : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeDurabilityBar.h + Private/Shell/EmergeDurabilityBar.cpp, static UFUNCTIONs.
//   Color int32: 0 green, 1 yellow, 2 red.
//
//   static float Fill01(float Current, float Max);   // return FMath::Clamp(Current / Max, 0.0f, 1.0f);
//   static int32 ColorTier(float Fill01);
//     // if (Fill01 > 0.5f) return 0;  if (Fill01 > 0.2f) return 1;  return 2;
//   static bool  IsBroken(float Current);   // return Current <= 0.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeDurabilityBar.h"

BEGIN_DEFINE_SPEC(FEmergeDurabilityBarSpec, "Emergence.Shell.DurabilityBar",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDurabilityBarSpec)
void FEmergeDurabilityBarSpec::Define()
{
	Describe("Fill, color, broken", [this]()
	{
		It("bar fill and color band", [this]()
		{
			TestEqual(TEXT("half fill"), UEmergeDurabilityBar::Fill01(50.f, 100.f), 0.5f);
			TestEqual(TEXT("over clamps 1"), UEmergeDurabilityBar::Fill01(150.f, 100.f), 1.0f);
			TestEqual(TEXT("0.8 green"), UEmergeDurabilityBar::ColorTier(0.8f), 0);
			TestEqual(TEXT("0.5 yellow"), UEmergeDurabilityBar::ColorTier(0.5f), 1);
			TestEqual(TEXT("0.3 yellow"), UEmergeDurabilityBar::ColorTier(0.3f), 1);
			TestEqual(TEXT("0.2 red"), UEmergeDurabilityBar::ColorTier(0.2f), 2);
			TestEqual(TEXT("0.1 red"), UEmergeDurabilityBar::ColorTier(0.1f), 2);
			TestTrue(TEXT("0 durability broken"), UEmergeDurabilityBar::IsBroken(0.f));
			TestFalse(TEXT("10 not broken"), UEmergeDurabilityBar::IsBroken(10.f));
		});
	});
}
#endif
