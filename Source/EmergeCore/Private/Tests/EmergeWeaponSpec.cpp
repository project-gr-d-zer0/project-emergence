// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon fire stats (deterministic).
// Fire cadence, magazine tracking, and climbing recoil — deterministic combat parameters.
// CONTRACT — NEW class UEmergeWeapon : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeWeapon.h + Private/Combat/EmergeWeapon.cpp, static UFUNCTIONs:
//   static float TimeBetweenShots(int32 RoundsPerMinute);   // return 60.0f / FMath::Max(1, RoundsPerMinute);
//   static int32 MagazineRemaining(int32 Loaded, int32 Fired); // return FMath::Max(0, Loaded - FMath::Max(0,Fired));
//   static float RecoilAtShot(float BaseRecoil, int32 ShotIndex);
//     // return BaseRecoil * (1.0f + FMath::Max(0, ShotIndex) * 0.1f);   // recoil climbs across a burst
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeWeapon.h"

BEGIN_DEFINE_SPEC(FEmergeWeaponSpec, "Emergence.Combat.Weapon",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeaponSpec)
void FEmergeWeaponSpec::Define()
{
	Describe("TimeBetweenShots (60 / RPM)", [this]()
	{
		It("matches exact golden values and guards zero RPM", [this]()
		{
			TestEqual(TEXT("600 RPM -> 0.1s"),  UEmergeWeapon::TimeBetweenShots(600), 0.1f);
			TestEqual(TEXT("60 RPM -> 1.0s"),   UEmergeWeapon::TimeBetweenShots(60), 1.0f);
			TestEqual(TEXT("0 RPM guards -> 60"),UEmergeWeapon::TimeBetweenShots(0), 60.0f);
		});
	});

	Describe("MagazineRemaining", [this]()
	{
		It("matches exact golden values and floors at 0", [this]()
		{
			TestEqual(TEXT("30 - 5 -> 25"),        UEmergeWeapon::MagazineRemaining(30, 5), 25);
			TestEqual(TEXT("over-fire -> 0"),      UEmergeWeapon::MagazineRemaining(30, 40), 0);
			TestEqual(TEXT("negative fired -> 30"),UEmergeWeapon::MagazineRemaining(30, -5), 30);
		});
	});

	Describe("RecoilAtShot (climbs 10% per shot)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("shot 0 -> base 2"),    UEmergeWeapon::RecoilAtShot(2.f, 0), 2.0f);
			TestEqual(TEXT("shot 5 -> 3"),         UEmergeWeapon::RecoilAtShot(2.f, 5), 3.0f);
			TestEqual(TEXT("negative shot -> base"),UEmergeWeapon::RecoilAtShot(2.f, -1), 2.0f);
		});
	});
}
#endif
