// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: spare magazine count (combat/gunplay).
// The total-ammo accounting for the HUD/inventory: rounds in reserve mags plus the loaded mag. Pairs with
// EmergeAmmoHud / EmergeMagCheck. Pure counting; deterministic.
// CONTRACT — NEW class UEmergeSpareMag : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeSpareMag.h + Private/Combat/EmergeSpareMag.cpp, static UFUNCTIONs.
//
//   static int32 TotalReserveRounds(int32 SpareMagCount, int32 MagSize);   // return SpareMagCount * MagSize;
//   static int32 TotalCarried(int32 LoadedRounds, int32 SpareMagCount, int32 MagSize);
//     // return LoadedRounds + SpareMagCount * MagSize;
//   static bool  HasSpareMag(int32 SpareMagCount);   // return SpareMagCount > 0;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeSpareMag.h"

BEGIN_DEFINE_SPEC(FEmergeSpareMagSpec, "Emergence.Combat.SpareMag",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSpareMagSpec)
void FEmergeSpareMagSpec::Define()
{
	Describe("Ammo accounting", [this]()
	{
		It("reserve and total carried rounds", [this]()
		{
			TestEqual(TEXT("3 mags x30 -> 90"), UEmergeSpareMag::TotalReserveRounds(3, 30), 90);
			TestEqual(TEXT("no spares -> 0"), UEmergeSpareMag::TotalReserveRounds(0, 30), 0);
			TestEqual(TEXT("15 loaded + 3x30 -> 105"), UEmergeSpareMag::TotalCarried(15, 3, 30), 105);
			TestEqual(TEXT("15 loaded, no spares -> 15"), UEmergeSpareMag::TotalCarried(15, 0, 30), 15);
			TestTrue(TEXT("has spares"), UEmergeSpareMag::HasSpareMag(2));
			TestFalse(TEXT("no spares"), UEmergeSpareMag::HasSpareMag(0));
		});
	});
}
#endif
