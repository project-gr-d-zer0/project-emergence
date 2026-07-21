// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: penetration vs armor class (combat/gunplay).
// Tarkov-style: a round penetrates armor when its penetration power meets the armor class requirement (class*10);
// on penetration it deals full damage, otherwise only blunt trauma scaled by how close it came. Distinct from the
// existing EmergeBallisticsMitigation - this is the pen-vs-class gate. TUNABLE design model, fixed + deterministic.
// CONTRACT — NEW class UEmergeArmorPen : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeArmorPen.h + Private/Combat/EmergeArmorPen.cpp, static UFUNCTIONs.
//
//   static float RequiredPen(int32 ArmorClass);   // return ArmorClass * 10.0f;
//   static bool  Penetrates(float PenPower, int32 ArmorClass);   // return PenPower >= RequiredPen(ArmorClass);
//   static float DamageMultiplier(float PenPower, int32 ArmorClass);
//     // if (Penetrates(PenPower, ArmorClass)) return 1.0f;                                   // full damage through
//     // return FMath::Clamp(PenPower / RequiredPen(ArmorClass), 0.0f, 1.0f) * 0.3f;          // blunt trauma only
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeArmorPen.h"

BEGIN_DEFINE_SPEC(FEmergeArmorPenSpec, "Emergence.Combat.ArmorPen",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeArmorPenSpec)
void FEmergeArmorPenSpec::Define()
{
	Describe("RequiredPen & Penetrates", [this]()
	{
		It("class*10 threshold, inclusive", [this]()
		{
			TestEqual(TEXT("class 4 needs 40"), UEmergeArmorPen::RequiredPen(4), 40.0f);
			TestEqual(TEXT("class 6 needs 60"), UEmergeArmorPen::RequiredPen(6), 60.0f);
			TestTrue(TEXT("45 pen beats class 4"), UEmergeArmorPen::Penetrates(45.f, 4));
			TestTrue(TEXT("exactly 40 penetrates class 4"), UEmergeArmorPen::Penetrates(40.f, 4));
			TestFalse(TEXT("30 stopped by class 4"), UEmergeArmorPen::Penetrates(30.f, 4));
		});
	});

	Describe("DamageMultiplier", [this]()
	{
		It("full on penetration, scaled blunt otherwise", [this]()
		{
			TestEqual(TEXT("penetration -> full"), UEmergeArmorPen::DamageMultiplier(45.f, 4), 1.0f);
			TestEqual(TEXT("half-threshold blunt -> 0.15"), UEmergeArmorPen::DamageMultiplier(20.f, 4), 0.15f);
			TestEqual(TEXT("no pen power -> 0"), UEmergeArmorPen::DamageMultiplier(0.f, 4), 0.0f);
		});
	});
}
#endif
