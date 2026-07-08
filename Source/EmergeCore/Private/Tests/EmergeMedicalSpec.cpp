// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: medical/bleeding (ties combat -> survival).
// Bleeding drains HP over time; bandages/medkits stop bleeding; healing restores HP (clamped 0..100).
// CONTRACT — NEW class UEmergeMedical : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeMedical.h + Private/Survival/EmergeMedical.cpp, static UFUNCTIONs:
//   static float BleedDamage(float BleedRate, float DeltaSeconds, bool bBleeding);
//     // return bBleeding ? BleedRate * FMath::Max(0.0f, DeltaSeconds) : 0.0f;
//   static float ApplyHeal(float CurrentHP, float HealAmount);
//     // return FMath::Clamp(CurrentHP + FMath::Max(0.0f, HealAmount), 0.0f, 100.0f);
//   static bool  StopsBleeding(int32 ItemType);   // bandage=1, medkit=2 stop bleeding; else false
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeMedical.h"

BEGIN_DEFINE_SPEC(FEmergeMedicalSpec, "Emergence.Survival.Medical",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMedicalSpec)
void FEmergeMedicalSpec::Define()
{
	Describe("BleedDamage (only while bleeding)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("rate2 * 5s bleeding -> 10"),  UEmergeMedical::BleedDamage(2.f, 5.f, true), 10.f);
			TestEqual(TEXT("not bleeding -> 0"),           UEmergeMedical::BleedDamage(2.f, 5.f, false), 0.f);
			TestEqual(TEXT("zero dt -> 0"),                UEmergeMedical::BleedDamage(3.f, 0.f, true), 0.f);
		});
	});

	Describe("ApplyHeal (clamped 0..100, never harms)", [this]()
	{
		It("matches exact golden values and clamps", [this]()
		{
			TestEqual(TEXT("50 + 30 -> 80"),          UEmergeMedical::ApplyHeal(50.f, 30.f), 80.f);
			TestEqual(TEXT("90 + 30 -> 100 (cap)"),    UEmergeMedical::ApplyHeal(90.f, 30.f), 100.f);
			TestEqual(TEXT("negative heal -> no harm"), UEmergeMedical::ApplyHeal(50.f, -10.f), 50.f);
			TestEqual(TEXT("0 + 25 -> 25"),            UEmergeMedical::ApplyHeal(0.f, 25.f), 25.f);
		});
	});

	Describe("StopsBleeding (bandage/medkit only)", [this]()
	{
		It("only medical items stop bleeding", [this]()
		{
			TestTrue(TEXT("bandage (1) stops"),  UEmergeMedical::StopsBleeding(1));
			TestTrue(TEXT("medkit (2) stops"),   UEmergeMedical::StopsBleeding(2));
			TestFalse(TEXT("nothing (0)"),       UEmergeMedical::StopsBleeding(0));
			TestFalse(TEXT("other item (3)"),    UEmergeMedical::StopsBleeding(3));
		});
	});
}
#endif
