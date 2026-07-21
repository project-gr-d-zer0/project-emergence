// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: weapon swap time (combat/gunplay).
// Switching weapons takes longer for heavier guns; a quick-swap (holster) is faster than a full draw; you can't
// swap mid-reload. TUNABLE design model, fixed deterministic.
// CONTRACT — NEW class UEmergeWeaponSwap : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeWeaponSwap.h + Private/Combat/EmergeWeaponSwap.cpp, static UFUNCTIONs.
//
//   static float SwapTimeSec(float BaseTime, float WeaponKg);   // return BaseTime * (1.0f + WeaponKg * 0.1f);
//   static float FastSwapTimeSec(float BaseTime, float WeaponKg);   // return SwapTimeSec(BaseTime, WeaponKg) * 0.6f;
//   static bool  CanSwap(bool bReloading);   // return !bReloading;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeWeaponSwap.h"

BEGIN_DEFINE_SPEC(FEmergeWeaponSwapSpec, "Emergence.Combat.WeaponSwap",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWeaponSwapSpec)
void FEmergeWeaponSwapSpec::Define()
{
	Describe("Swap timing", [this]()
	{
		It("heavier is slower; fast-swap is 60%; blocked while reloading", [this]()
		{
			TestEqual(TEXT("weightless -> base"), UEmergeWeaponSwap::SwapTimeSec(1.0f, 0.f), 1.0f);
			TestEqual(TEXT("5kg -> 1.5"), UEmergeWeaponSwap::SwapTimeSec(1.0f, 5.f), 1.5f);
			TestEqual(TEXT("10kg -> 2.0"), UEmergeWeaponSwap::SwapTimeSec(1.0f, 10.f), 2.0f);
			TestEqual(TEXT("fast swap 5kg -> 0.9"), UEmergeWeaponSwap::FastSwapTimeSec(1.0f, 5.f), 0.9f);
			TestTrue(TEXT("can swap normally"), UEmergeWeaponSwap::CanSwap(false));
			TestFalse(TEXT("cannot swap mid-reload"), UEmergeWeaponSwap::CanSwap(true));
		});
	});
}
#endif
