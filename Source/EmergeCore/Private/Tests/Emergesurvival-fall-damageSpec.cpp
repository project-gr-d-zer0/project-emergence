// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fall damage.
// Landing hard hurts; below a safe speed it is free. Ties movement -> vitals.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeVitals (do NOT rename/duplicate it;
//   keep Deplete/IsStarving/StarvationDamage exactly), in the existing
//   Public/Survival/EmergeVitals.h + Private/Survival/EmergeVitals.cpp:
//   static float FallDamage(float ImpactSpeed);
//     // if (ImpactSpeed <= 800.0f) return 0.0f;                      // safe landing (cm/s)
//     // return FMath::Min(100.0f, (ImpactSpeed - 800.0f) * 0.05f);   // caps at lethal 100
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeVitals.h"

BEGIN_DEFINE_SPEC(FEmergeFallDamageSpec, "Emergence.Survival.FallDamage",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFallDamageSpec)
void FEmergeFallDamageSpec::Define()
{
	Describe("FallDamage (safe under 800 cm/s, capped at 100)", [this]()
	{
		It("matches exact golden values and boundaries", [this]()
		{
			TestEqual(TEXT("gentle 500 -> 0"), UEmergeVitals::FallDamage(500.f), 0.f);
			TestEqual(TEXT("boundary 800 -> 0"), UEmergeVitals::FallDamage(800.f), 0.f);
			TestEqual(TEXT("1000 -> 10"), UEmergeVitals::FallDamage(1000.f), 10.f);
			TestEqual(TEXT("2000 -> 60"), UEmergeVitals::FallDamage(2000.f), 60.f);
			TestEqual(TEXT("3000 -> 100 (cap)"), UEmergeVitals::FallDamage(3000.f), 100.f);
		});
	});
}
#endif
