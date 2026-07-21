// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hitbox zone damage multiplier (combat/gunplay).
// Tarkov-style zoned damage: where the round lands scales the damage - head lethal, stomach elevated, limbs
// reduced. TUNABLE design multipliers, fixed deterministic model. Composes with EmergeBallisticDropoff / ArmorPen.
// CONTRACT — NEW class UEmergeHitZone : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeHitZone.h + Private/Combat/EmergeHitZone.cpp, static UFUNCTIONs.
//   Zone int32: 0 Head, 1 Thorax, 2 Stomach, 3 Arms, 4 Legs.
//
//   static float ZoneMultiplier(int32 Zone);
//     // switch: 0 -> 2.0f; 1 -> 1.0f; 2 -> 1.5f; 3 -> 0.7f; 4 -> 0.7f; default -> 1.0f;
//   static float ApplyZone(float BaseDamage, int32 Zone);   // return BaseDamage * ZoneMultiplier(Zone);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeHitZone.h"

BEGIN_DEFINE_SPEC(FEmergeHitZoneSpec, "Emergence.Combat.HitZone",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHitZoneSpec)
void FEmergeHitZoneSpec::Define()
{
	Describe("ZoneMultiplier", [this]()
	{
		It("head lethal, stomach elevated, limbs reduced", [this]()
		{
			TestEqual(TEXT("head 2.0"), UEmergeHitZone::ZoneMultiplier(0), 2.0f);
			TestEqual(TEXT("thorax 1.0"), UEmergeHitZone::ZoneMultiplier(1), 1.0f);
			TestEqual(TEXT("stomach 1.5"), UEmergeHitZone::ZoneMultiplier(2), 1.5f);
			TestEqual(TEXT("arms 0.7"), UEmergeHitZone::ZoneMultiplier(3), 0.7f);
			TestEqual(TEXT("legs 0.7"), UEmergeHitZone::ZoneMultiplier(4), 0.7f);
			TestEqual(TEXT("unknown 1.0"), UEmergeHitZone::ZoneMultiplier(9), 1.0f);
		});
	});

	Describe("ApplyZone", [this]()
	{
		It("scales base damage by the zone", [this]()
		{
			TestEqual(TEXT("50 to head -> 100"), UEmergeHitZone::ApplyZone(50.f, 0), 100.0f);
			TestEqual(TEXT("50 to thorax -> 50"), UEmergeHitZone::ApplyZone(50.f, 1), 50.0f);
			TestEqual(TEXT("50 to legs -> 35"), UEmergeHitZone::ApplyZone(50.f, 4), 35.0f);
		});
	});
}
#endif
