// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon spread at range.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeWeapon (do NOT rename/duplicate it;
//   keep TimeBetweenShots/MagazineRemaining/RecoilAtShot/heat methods exactly), in the existing
//   Public/Combat/EmergeWeapon.h + Private/Combat/EmergeWeapon.cpp:
//   static float SpreadAtRange(float BaseMOA, float RangeM, bool bAiming);
//     // return BaseMOA * (1.0f + FMath::Max(0.0f, RangeM) / 100.0f) * (bAiming ? 0.5f : 1.0f);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeWeapon.h"

BEGIN_DEFINE_SPEC(FEmergeWeaponSpreadSpec, "Emergence.Combat.WeaponSpread",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeaponSpreadSpec)
void FEmergeWeaponSpreadSpec::Define()
{
	Describe("SpreadAtRange (grows with range, aiming halves)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("point blank -> base"), UEmergeWeapon::SpreadAtRange(2.f, 0.f, false), 2.f);
			TestEqual(TEXT("100m -> 4"), UEmergeWeapon::SpreadAtRange(2.f, 100.f, false), 4.f);
			TestEqual(TEXT("100m aiming -> 2"), UEmergeWeapon::SpreadAtRange(2.f, 100.f, true), 2.f);
			TestEqual(TEXT("200m -> 6"), UEmergeWeapon::SpreadAtRange(2.f, 200.f, false), 6.f);
		});
	});
}
#endif
