// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: chambered round (combat/gunplay).
// The "+1 in the chamber" accounting: a loaded weapon holds a full mag plus one chambered round, so total
// capacity is MagSize+1. Pairs with EmergeAmmoHud / EmergeSpareMag. Pure counting; deterministic.
// CONTRACT — NEW class UEmergeChamber : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeChamber.h + Private/Combat/EmergeChamber.cpp, static UFUNCTIONs.
//
//   static int32 TotalWithChamber(int32 MagRounds, bool bChambered);   // return MagRounds + (bChambered ? 1 : 0);
//   static int32 MaxCapacity(int32 MagSize);                            // return MagSize + 1;
//   static bool  CanChamber(int32 MagRounds, bool bAlreadyChambered);   // return MagRounds > 0 && !bAlreadyChambered;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeChamber.h"

BEGIN_DEFINE_SPEC(FEmergeChamberSpec, "Emergence.Combat.Chamber",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeChamberSpec)
void FEmergeChamberSpec::Define()
{
	Describe("Chamber accounting", [this]()
	{
		It("+1 in the chamber and max capacity", [this]()
		{
			TestEqual(TEXT("30 + chambered -> 31"), UEmergeChamber::TotalWithChamber(30, true), 31);
			TestEqual(TEXT("30 no chamber -> 30"), UEmergeChamber::TotalWithChamber(30, false), 30);
			TestEqual(TEXT("empty mag but chambered -> 1"), UEmergeChamber::TotalWithChamber(0, true), 1);
			TestEqual(TEXT("30-mag max cap 31"), UEmergeChamber::MaxCapacity(30), 31);
			TestTrue(TEXT("can chamber with rounds"), UEmergeChamber::CanChamber(5, false));
			TestFalse(TEXT("cannot chamber empty"), UEmergeChamber::CanChamber(0, false));
			TestFalse(TEXT("cannot re-chamber"), UEmergeChamber::CanChamber(5, true));
		});
	});
}
#endif
