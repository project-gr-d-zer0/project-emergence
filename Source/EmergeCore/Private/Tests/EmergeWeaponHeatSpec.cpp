// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon heat + malfunction chance.
// Heat builds per shot and cools over time; malfunction chance is ZERO above 93% durability while
// cool, and rises monotonically with wear and heat. A jam mid-horde is the signature terror moment.
// CONTRACT — ADD THREE static UFUNCTIONs to the EXISTING class UEmergeWeapon (do NOT rename/duplicate
//   it; keep TimeBetweenShots/MagazineRemaining/RecoilAtShot exactly), in the existing
//   Public/Combat/EmergeWeapon.h + Private/Combat/EmergeWeapon.cpp:
//   static float HeatAfterShots(float Heat, int32 Shots, float HeatPerShot);
//     // return FMath::Max(0.0f, Heat) + FMath::Max(0, Shots) * FMath::Max(0.0f, HeatPerShot);
//   static float HeatAfterCooling(float Heat, float CoolPerSec, float DeltaSeconds);
//     // return FMath::Max(0.0f, Heat - FMath::Max(0.0f, CoolPerSec) * FMath::Max(0.0f, DeltaSeconds));
//   static float MalfunctionChance(float DurabilityPct, float Heat);
//     // float C = 0.0f;
//     // if (DurabilityPct < 93.0f) { C += (93.0f - FMath::Max(0.0f, DurabilityPct)) * 0.001f; }
//     // if (Heat >= 50.0f) { C += (Heat - 50.0f) * 0.002f; }
//     // return FMath::Clamp(C, 0.0f, 1.0f);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeWeapon.h"

BEGIN_DEFINE_SPEC(FEmergeWeaponHeatSpec, "Emergence.Combat.WeaponHeat",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeaponHeatSpec)
void FEmergeWeaponHeatSpec::Define()
{
	Describe("Heat accumulation and cooling", [this]()
	{
		It("adds per shot and cools toward 0", [this]()
		{
			TestEqual(TEXT("0 + 10 shots x 2 -> 20"), UEmergeWeapon::HeatAfterShots(0.f, 10, 2.f), 20.f);
			TestEqual(TEXT("cool 20 by 5/s x 2s -> 10"), UEmergeWeapon::HeatAfterCooling(20.f, 5.f, 2.f), 10.f);
			TestEqual(TEXT("over-cool floors at 0"), UEmergeWeapon::HeatAfterCooling(5.f, 5.f, 2.f), 0.f);
		});
	});

	Describe("MalfunctionChance (0 above 93% while cool, monotone below)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("pristine cool -> 0"), UEmergeWeapon::MalfunctionChance(100.f, 0.f), 0.f);
			TestEqual(TEXT("93% cool -> 0"), UEmergeWeapon::MalfunctionChance(93.f, 0.f), 0.f);
			TestEqual(TEXT("83% cool -> 0.01"), UEmergeWeapon::MalfunctionChance(83.f, 0.f), 0.01f);
			TestEqual(TEXT("93% hot 100 -> 0.1"), UEmergeWeapon::MalfunctionChance(93.f, 100.f), 0.1f);
			TestEqual(TEXT("73% hot 100 -> 0.12"), UEmergeWeapon::MalfunctionChance(73.f, 100.f), 0.12f);
		});
	});
}
#endif
