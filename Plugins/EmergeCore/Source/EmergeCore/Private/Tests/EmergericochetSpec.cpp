// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ricochet angle (combat/gunplay).
// A round striking a hard surface at a shallow angle (measured from the surface plane) ricochets instead of
// penetrating, keeping more velocity the shallower it is. Composes with EmergeBallisticDropoff. TUNABLE threshold,
// fixed deterministic model.
// CONTRACT — NEW class UEmergeRicochet : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeRicochet.h + Private/Combat/EmergeRicochet.cpp, static UFUNCTIONs.
//
//   static bool  WouldRicochet(float ImpactAngleDeg, float RicochetThreshold);   // return ImpactAngleDeg <= RicochetThreshold;
//   static float RetainedVelocity01(float ImpactAngleDeg, float RicochetThreshold);
//     // if (ImpactAngleDeg > RicochetThreshold) return 0.0f;   return 1.0f - ImpactAngleDeg / RicochetThreshold;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeRicochet.h"

BEGIN_DEFINE_SPEC(FEmergeRicochetSpec, "Emergence.Combat.Ricochet",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeRicochetSpec)
void FEmergeRicochetSpec::Define()
{
	Describe("Ricochet", [this]()
	{
		It("shallow angles ricochet and retain velocity", [this]()
		{
			TestTrue(TEXT("10deg ricochets under 30 threshold"), UEmergeRicochet::WouldRicochet(10.f, 30.f));
			TestTrue(TEXT("at threshold ricochets"), UEmergeRicochet::WouldRicochet(30.f, 30.f));
			TestFalse(TEXT("45deg penetrates"), UEmergeRicochet::WouldRicochet(45.f, 30.f));
			TestEqual(TEXT("grazing (0deg) retains all"), UEmergeRicochet::RetainedVelocity01(0.f, 30.f), 1.0f);
			TestEqual(TEXT("halfway -> 0.5"), UEmergeRicochet::RetainedVelocity01(15.f, 30.f), 0.5f);
			TestEqual(TEXT("at threshold -> 0"), UEmergeRicochet::RetainedVelocity01(30.f, 30.f), 0.0f);
			TestEqual(TEXT("penetrating angle -> 0"), UEmergeRicochet::RetainedVelocity01(45.f, 30.f), 0.0f);
		});
	});
}
#endif
