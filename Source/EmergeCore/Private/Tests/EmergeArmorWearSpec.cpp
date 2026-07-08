// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: armor durability degradation.
// Worn armor protects less; penetrating rounds chew 15% less durability than blocked ones (they pass
// through); repair permanently lowers max durability. Closes the armor<->durability<->economy loop.
// CONTRACT — NEW class UEmergeArmorWear : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeArmorWear.h + Private/Combat/EmergeArmorWear.cpp, static UFUNCTIONs:
//   static float EffectiveProtection(float ArmorValue, float CurDurability, float MaxDurability);
//     // if (MaxDurability <= 0.0f) return 0.0f;
//     // return ArmorValue * FMath::Clamp(CurDurability / MaxDurability, 0.0f, 1.0f);
//   static float DurabilityLoss(bool bPenetrated, float AmmoArmorDamage);
//     // const float Base = FMath::Max(0.0f, AmmoArmorDamage) * (bPenetrated ? 0.85f : 1.0f);
//     // return FMath::Max(1.0f, Base);   // minimum 1 durability damage per hit
//   static float MaxAfterRepair(float MaxDurability);
//     // return FMath::Max(0.0f, MaxDurability * 0.9f);   // each repair costs 10% of max
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeArmorWear.h"

BEGIN_DEFINE_SPEC(FEmergeArmorWearSpec, "Emergence.Combat.ArmorWear",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeArmorWearSpec)
void FEmergeArmorWearSpec::Define()
{
	Describe("EffectiveProtection (wear degrades protection)", [this]()
	{
		It("scales armor by durability fraction, guards zero max", [this]()
		{
			TestEqual(TEXT("80 armor at 50/100 -> 40"), UEmergeArmorWear::EffectiveProtection(80.f, 50.f, 100.f), 40.f);
			TestEqual(TEXT("full durability -> full armor"), UEmergeArmorWear::EffectiveProtection(80.f, 100.f, 100.f), 80.f);
			TestEqual(TEXT("zero max -> 0"), UEmergeArmorWear::EffectiveProtection(80.f, 50.f, 0.f), 0.f);
			TestEqual(TEXT("overfull clamps to 1"), UEmergeArmorWear::EffectiveProtection(80.f, 150.f, 100.f), 80.f);
		});
	});

	Describe("DurabilityLoss (pen chews 15% less, min 1)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("blocked 20 -> 20"), UEmergeArmorWear::DurabilityLoss(false, 20.f), 20.f);
			TestEqual(TEXT("penetrated 20 -> 17"), UEmergeArmorWear::DurabilityLoss(true, 20.f), 17.f);
			TestEqual(TEXT("tiny hit floors at 1"), UEmergeArmorWear::DurabilityLoss(true, 0.5f), 1.f);
		});
	});

	Describe("MaxAfterRepair (repair costs 10% of max)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("100 -> 90"), UEmergeArmorWear::MaxAfterRepair(100.f), 90.f);
			TestEqual(TEXT("90 -> 81"), UEmergeArmorWear::MaxAfterRepair(90.f), 81.f);
			TestEqual(TEXT("negative floors at 0"), UEmergeArmorWear::MaxAfterRepair(-10.f), 0.f);
		});
	});
}
#endif
