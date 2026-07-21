// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: shock / consciousness (DayZ-style).
// Shock is a 0..100 consciousness pool: it refills over time (fast awake, slow out), and DRAINS via shock
// damage when blood runs low (the closer to the fatal floor, the faster). Broken legs and melee inflict
// shock too. Numbers are exact from decompiled DayZ playerconstants.c: refill 5/s conscious, 1/s
// unconscious (x0.8 if also critical blood); shock damage ramps 5.5/s (blood 3000) -> 6.25/s (blood 2500),
// zero above 3000; broken-legs walk shock 4/6/8 by leg tier; melee shock light 10 / heavy 25.
// CONTRACT — NEW class UEmergeShock : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeShock.h + Private/Survival/EmergeShock.cpp, static UFUNCTIONs:
//
//   static float ShockRefillPerSec(bool bConscious, bool bUnconsciousCriticalBlood);
//     // if (bConscious) return 5.0f;
//     // return bUnconsciousCriticalBlood ? 1.0f * 0.8f : 1.0f;
//
//   static float ShockDamagePerSec(float CurrentBlood);
//     // No shock damage until blood drops to the high threshold (3000); ramps to max at the low one (2500).
//     // if (CurrentBlood > 3000.f) return 0.0f;
//     // if (CurrentBlood <= 2500.f) return 6.25f;
//     // const float T = (3000.f - CurrentBlood) / 500.f;            // 0 at 3000, 1 at 2500
//     // return 5.5f + T * (6.25f - 5.5f);
//
//   static float BrokenLegsWalkShock(int32 LegTier);
//     // Shock inflicted per walk update with broken legs. LegTier: 0=almost-healthy->4.0, 1=mid->6.0,
//     // 2=low-health->8.0; any other -> 4.0.
//
//   static float MeleeShock(bool bHeavy);
//     // return bHeavy ? 25.0f : 10.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeShock.h"

BEGIN_DEFINE_SPEC(FEmergeShockSpec, "Emergence.Survival.Shock",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeShockSpec)
void FEmergeShockSpec::Define()
{
	Describe("ShockRefillPerSec", [this]()
	{
		It("fast awake, slow out, slower when unconscious+critical blood", [this]()
		{
			TestEqual(TEXT("conscious -> 5"), UEmergeShock::ShockRefillPerSec(true, false), 5.0f);
			TestEqual(TEXT("conscious ignores blood -> 5"), UEmergeShock::ShockRefillPerSec(true, true), 5.0f);
			TestEqual(TEXT("unconscious -> 1"), UEmergeShock::ShockRefillPerSec(false, false), 1.0f);
			TestEqual(TEXT("uncon+critical -> 0.8"), UEmergeShock::ShockRefillPerSec(false, true), 0.8f);
		});
	});

	Describe("ShockDamagePerSec", [this]()
	{
		It("zero above 3000, ramps 5.5->6.25 across 3000..2500, capped below", [this]()
		{
			TestEqual(TEXT("healthy blood -> 0"), UEmergeShock::ShockDamagePerSec(3200.f), 0.0f);
			TestEqual(TEXT("at 3000 -> 5.5"), UEmergeShock::ShockDamagePerSec(3000.f), 5.5f);
			TestEqual(TEXT("midway 2750 -> 5.875"), UEmergeShock::ShockDamagePerSec(2750.f), 5.875f);
			TestEqual(TEXT("at 2500 -> 6.25"), UEmergeShock::ShockDamagePerSec(2500.f), 6.25f);
			TestEqual(TEXT("below 2500 -> 6.25"), UEmergeShock::ShockDamagePerSec(2000.f), 6.25f);
		});
	});

	Describe("Broken legs + melee shock", [this]()
	{
		It("leg tiers and melee weight (exact DayZ values)", [this]()
		{
			TestEqual(TEXT("legs almost-healthy -> 4"), UEmergeShock::BrokenLegsWalkShock(0), 4.0f);
			TestEqual(TEXT("legs mid -> 6"), UEmergeShock::BrokenLegsWalkShock(1), 6.0f);
			TestEqual(TEXT("legs low -> 8"), UEmergeShock::BrokenLegsWalkShock(2), 8.0f);
			TestEqual(TEXT("light melee -> 10"), UEmergeShock::MeleeShock(false), 10.0f);
			TestEqual(TEXT("heavy melee -> 25"), UEmergeShock::MeleeShock(true), 25.0f);
		});
	});
}
#endif
