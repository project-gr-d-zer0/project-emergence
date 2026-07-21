// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: damage number popup (shell/combat HUD).
// SHELL stratum: the floating damage number - scale and color band grow with the hit's size relative to a
// reference, and big hits flag as crits. Pure view banding; deterministic.
// CONTRACT — NEW class UEmergeDamagePopup : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeDamagePopup.h + Private/Shell/EmergeDamagePopup.cpp, static UFUNCTIONs.
//   Color int32: 0 low, 1 normal, 2 high, 3 crit.
//
//   static float PopupScale(float Damage, float RefDamage);
//     // return FMath::Clamp(Damage / RefDamage, 0.5f, 2.0f);
//   static int32 SeverityColor(float Damage, float RefDamage);
//     // const float f = Damage / RefDamage;
//     // if (f < 0.5f) return 0;  if (f < 1.0f) return 1;  if (f < 2.0f) return 2;  return 3;
//   static bool  IsCrit(float Damage, float RefDamage);   // return Damage >= RefDamage * 2.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeDamagePopup.h"

BEGIN_DEFINE_SPEC(FEmergeDamagePopupSpec, "Emergence.Shell.DamagePopup",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDamagePopupSpec)
void FEmergeDamagePopupSpec::Define()
{
	Describe("PopupScale", [this]()
	{
		It("scales with relative damage, clamped", [this]()
		{
			TestEqual(TEXT("reference -> 1.0"), UEmergeDamagePopup::PopupScale(50.f, 50.f), 1.0f);
			TestEqual(TEXT("double -> 2.0"), UEmergeDamagePopup::PopupScale(100.f, 50.f), 2.0f);
			TestEqual(TEXT("small clamps 0.5"), UEmergeDamagePopup::PopupScale(10.f, 50.f), 0.5f);
			TestEqual(TEXT("huge clamps 2.0"), UEmergeDamagePopup::PopupScale(500.f, 50.f), 2.0f);
		});
	});

	Describe("SeverityColor & IsCrit", [this]()
	{
		It("color bands and crit flag", [this]()
		{
			TestEqual(TEXT("0.2x low"), UEmergeDamagePopup::SeverityColor(10.f, 50.f), 0);
			TestEqual(TEXT("0.6x normal"), UEmergeDamagePopup::SeverityColor(30.f, 50.f), 1);
			TestEqual(TEXT("1.5x high"), UEmergeDamagePopup::SeverityColor(75.f, 50.f), 2);
			TestEqual(TEXT("3x crit color"), UEmergeDamagePopup::SeverityColor(150.f, 50.f), 3);
			TestTrue(TEXT("2x is crit"), UEmergeDamagePopup::IsCrit(100.f, 50.f));
			TestFalse(TEXT("just under not crit"), UEmergeDamagePopup::IsCrit(99.f, 50.f));
		});
	});
}
#endif
