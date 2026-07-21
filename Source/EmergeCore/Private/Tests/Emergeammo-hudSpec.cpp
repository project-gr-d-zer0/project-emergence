// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ammo/reload HUD view-model (Tarkov-style).
// SHELL stratum: the magazine + reserve readout and the reload arithmetic that drives the ammo counter - mag
// fill, empty/low warnings, and how a reload tops the mag from the reserve pool. Pure view/inventory math, no UMG.
// CONTRACT — NEW class UEmergeAmmoHud : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeAmmoHud.h + Private/Shell/EmergeAmmoHud.cpp, static UFUNCTIONs.
//
//   static float MagFill01(int32 Current, int32 MagSize);   // return FMath::Clamp((float)Current / (float)MagSize, 0.f, 1.f);
//   static bool  IsEmpty(int32 Current);                    // return Current <= 0;
//   static bool  LowAmmo(int32 Current, int32 MagSize);     // return Current > 0 && (Current * 4) <= MagSize; // <=25%, not empty
//   static int32 MagAfterReload(int32 Reserve, int32 Current, int32 MagSize);
//     // const int32 Taken = FMath::Min(MagSize - Current, Reserve); return Current + Taken;
//   static int32 ReserveAfterReload(int32 Reserve, int32 Current, int32 MagSize);
//     // const int32 Taken = FMath::Min(MagSize - Current, Reserve); return Reserve - Taken;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeAmmoHud.h"

BEGIN_DEFINE_SPEC(FEmergeAmmoHudSpec, "Emergence.Shell.AmmoHud",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAmmoHudSpec)
void FEmergeAmmoHudSpec::Define()
{
	Describe("Mag fill & warnings", [this]()
	{
		It("fill fraction, empty and low (<=25%, not empty)", [this]()
		{
			TestEqual(TEXT("15/30 -> 0.5"), UEmergeAmmoHud::MagFill01(15, 30), 0.5f);
			TestEqual(TEXT("full"), UEmergeAmmoHud::MagFill01(30, 30), 1.0f);
			TestEqual(TEXT("empty fill 0"), UEmergeAmmoHud::MagFill01(0, 30), 0.0f);
			TestTrue(TEXT("0 is empty"), UEmergeAmmoHud::IsEmpty(0));
			TestFalse(TEXT("1 not empty"), UEmergeAmmoHud::IsEmpty(1));
			TestTrue(TEXT("7/30 low (<=25%)"), UEmergeAmmoHud::LowAmmo(7, 30));
			TestFalse(TEXT("8/30 not low"), UEmergeAmmoHud::LowAmmo(8, 30));
			TestFalse(TEXT("empty is not 'low'"), UEmergeAmmoHud::LowAmmo(0, 30));
		});
	});

	Describe("Reload arithmetic", [this]()
	{
		It("tops the mag from reserve, capping at need and available", [this]()
		{
			TestEqual(TEXT("plenty: mag 10->30"), UEmergeAmmoHud::MagAfterReload(60, 10, 30), 30);
			TestEqual(TEXT("plenty: reserve 60->40"), UEmergeAmmoHud::ReserveAfterReload(60, 10, 30), 40);
			TestEqual(TEXT("scarce: mag 10->15"), UEmergeAmmoHud::MagAfterReload(5, 10, 30), 15);
			TestEqual(TEXT("scarce: reserve 5->0"), UEmergeAmmoHud::ReserveAfterReload(5, 10, 30), 0);
			TestEqual(TEXT("full mag unchanged"), UEmergeAmmoHud::MagAfterReload(60, 30, 30), 30);
		});
	});
}
#endif
