// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: magazine check state (combat/gunplay).
// Tarkov/DayZ-style: checking a mag gives only a rough fill BAND (empty/low/half/high/full), not an exact count,
// so the player must judge. Pairs with EmergeAmmoHud (which knows the true count). Pure banding math; deterministic.
// CONTRACT — NEW class UEmergeMagCheck : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeMagCheck.h + Private/Combat/EmergeMagCheck.cpp, static UFUNCTIONs.
//   Band int32: 0 Empty, 1 Low, 2 Half, 3 High, 4 Full.
//
//   static int32 AmmoBand(int32 Current, int32 MagSize);
//     // if (Current <= 0) return 0;   if (Current >= MagSize) return 4;
//     // const float f = (float)Current / (float)MagSize;
//     // if (f < 0.25f) return 1;   if (f < 0.75f) return 2;   return 3;
//   static bool NeedsReload(int32 Current, int32 MagSize);   // return AmmoBand(Current, MagSize) <= 1;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeMagCheck.h"

BEGIN_DEFINE_SPEC(FEmergeMagCheckSpec, "Emergence.Combat.MagCheck",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMagCheckSpec)
void FEmergeMagCheckSpec::Define()
{
	Describe("AmmoBand", [this]()
	{
		It("bins mag fill into rough bands", [this]()
		{
			TestEqual(TEXT("empty"), UEmergeMagCheck::AmmoBand(0, 30), 0);
			TestEqual(TEXT("5/30 low"), UEmergeMagCheck::AmmoBand(5, 30), 1);
			TestEqual(TEXT("15/30 half"), UEmergeMagCheck::AmmoBand(15, 30), 2);
			TestEqual(TEXT("25/30 high"), UEmergeMagCheck::AmmoBand(25, 30), 3);
			TestEqual(TEXT("29/30 high"), UEmergeMagCheck::AmmoBand(29, 30), 3);
			TestEqual(TEXT("30/30 full"), UEmergeMagCheck::AmmoBand(30, 30), 4);
		});
	});

	Describe("NeedsReload", [this]()
	{
		It("empty or low prompts a reload", [this]()
		{
			TestTrue(TEXT("empty needs reload"), UEmergeMagCheck::NeedsReload(0, 30));
			TestTrue(TEXT("low needs reload"), UEmergeMagCheck::NeedsReload(5, 30));
			TestFalse(TEXT("half is fine"), UEmergeMagCheck::NeedsReload(15, 30));
		});
	});
}
#endif
