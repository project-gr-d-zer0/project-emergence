// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ammo type view-model (shell/combat).
// SHELL stratum: classifies the loaded ammo type for the HUD icon/color and exposes its combat traits (AP flag,
// flesh-damage modifier) that feed EmergeArmorPen / EmergeBallisticDropoff. TUNABLE design values, deterministic.
// CONTRACT — NEW class UEmergeAmmoType : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeAmmoType.h + Private/Shell/EmergeAmmoType.cpp, static UFUNCTIONs.
//   AmmoType int32: 0 FMJ, 1 HollowPoint, 2 ArmorPiercing, 3 Tracer.
//
//   static bool  IsArmorPiercing(int32 AmmoType);   // return AmmoType == 2;
//   static int32 IconColor(int32 AmmoType);
//     // switch: 0 -> 0; 1 -> 1; 2 -> 2; 3 -> 3; default -> 0;
//   static float DamageModifier(int32 AmmoType);
//     // switch: 1 -> 1.3f; 2 -> 0.9f; default (0,3) -> 1.0f;   // HP more flesh dmg, AP less but penetrates
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeAmmoType.h"

BEGIN_DEFINE_SPEC(FEmergeAmmoTypeSpec, "Emergence.Shell.AmmoType",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeAmmoTypeSpec)
void FEmergeAmmoTypeSpec::Define()
{
	Describe("AmmoType traits", [this]()
	{
		It("AP flag, icon color, flesh-damage modifier", [this]()
		{
			TestTrue(TEXT("AP is armor-piercing"), UEmergeAmmoType::IsArmorPiercing(2));
			TestFalse(TEXT("FMJ is not AP"), UEmergeAmmoType::IsArmorPiercing(0));
			TestEqual(TEXT("FMJ color 0"), UEmergeAmmoType::IconColor(0), 0);
			TestEqual(TEXT("HP color 1"), UEmergeAmmoType::IconColor(1), 1);
			TestEqual(TEXT("AP color 2"), UEmergeAmmoType::IconColor(2), 2);
			TestEqual(TEXT("tracer color 3"), UEmergeAmmoType::IconColor(3), 3);
			TestEqual(TEXT("FMJ dmg 1.0"), UEmergeAmmoType::DamageModifier(0), 1.0f);
			TestEqual(TEXT("HP dmg 1.3"), UEmergeAmmoType::DamageModifier(1), 1.3f);
			TestEqual(TEXT("AP dmg 0.9"), UEmergeAmmoType::DamageModifier(2), 0.9f);
			TestEqual(TEXT("tracer dmg 1.0"), UEmergeAmmoType::DamageModifier(3), 1.0f);
		});
	});
}
#endif
