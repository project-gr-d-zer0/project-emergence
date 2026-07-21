// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: combat shot resolution INTEGRATION (combat).
// VERTICAL-DEPTH module: resolves ONE shot end-to-end by COMPOSING existing combat modules - range falloff
// (EmergeBallisticDropoff) x ammo type (EmergeAmmoType) x armor penetration (EmergeArmorPen) x hit zone
// (EmergeHitZone) -> final damage. This is the missing middle-rung that turns 4 leaf formulas into a real
// shot pipeline. Tested MOSTLY METAMORPHICALLY (headshots hurt more, closer hurts more, blocked rounds hurt
// less) + a couple tolerance-safe exact goldens.
// CONTRACT — NEW class UEmergeCombatResolve : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeCombatResolve.h + Private/Combat/EmergeCombatResolve.cpp, static UFUNCTIONs.
//   The .cpp must #include EXACTLY these (mind the subdirs):
//     #include "Combat/EmergeBallisticDropoff.h"
//     #include "Shell/EmergeAmmoType.h"      // NOTE: AmmoType lives under Shell/, not Combat/
//     #include "Combat/EmergeArmorPen.h"
//     #include "Combat/EmergeHitZone.h"
//
//   static float ResolveDamage(float BaseDamage, float Distance, float FalloffStart, float FalloffEnd,
//                              float MinMult, int32 AmmoType, float PenPower, int32 ArmorClass, int32 HitZone);
//     // float d = UEmergeBallisticDropoff::DamageAtRange(BaseDamage, Distance, FalloffStart, FalloffEnd, MinMult);
//     // d *= UEmergeAmmoType::DamageModifier(AmmoType);
//     // d *= UEmergeArmorPen::DamageMultiplier(PenPower, ArmorClass);
//     // d *= UEmergeHitZone::ZoneMultiplier(HitZone);
//     // return d;
//
// Deterministic; tolerance-safe exact goldens + metamorphic relations.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeCombatResolve.h"

BEGIN_DEFINE_SPEC(FEmergeCombatResolveSpec, "Emergence.Combat.CombatResolve",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCombatResolveSpec)
void FEmergeCombatResolveSpec::Define()
{
	// helper params: base100, close (dist10, falloff 20..100, min .5), FMJ(0), pen45 vs class4 (penetrates), thorax(1)
	Describe("Exact composition (tolerance)", [this]()
	{
		It("full-power thorax hit at close range = base", [this]()
		{
			TestEqual(TEXT("clean thorax hit -> 100"),
				UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1), 100.0f, 0.01f);
			TestEqual(TEXT("headshot -> 200"),
				UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 0), 200.0f, 0.01f);
			TestEqual(TEXT("midrange thorax -> 75"),
				UEmergeCombatResolve::ResolveDamage(100.f, 60.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1), 75.0f, 0.01f);
		});
	});

	Describe("Metamorphic relations", [this]()
	{
		It("headshots > body, closer > farther, blocked < penetrating, bounded", [this]()
		{
			const float head   = UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 0);
			const float thorax = UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1);
			const float legs   = UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 4);
			TestTrue(TEXT("head > thorax > legs"), head > thorax && thorax > legs);

			const float close = UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1);
			const float far   = UEmergeCombatResolve::ResolveDamage(100.f, 100.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1);
			TestTrue(TEXT("closer hits harder"), close > far);

			const float pen     = UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1);
			const float blocked = UEmergeCombatResolve::ResolveDamage(100.f, 10.f, 20.f, 100.f, 0.5f, 0, 30.f, 4, 1);
			TestTrue(TEXT("penetrating round does more than blocked"), pen > blocked);

			// bounded / non-negative across a sweep of distance
			for (int32 dist = 0; dist <= 150; dist += 25)
			{
				const float d = UEmergeCombatResolve::ResolveDamage(100.f, (float)dist, 20.f, 100.f, 0.5f, 0, 45.f, 4, 1);
				TestTrue(TEXT("damage non-negative"), d >= 0.f);
			}
		});
	});
}
#endif
